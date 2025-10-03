#include "../include/Client.hpp"
#include "../include/CGIExecutor.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>


Client::Client()
{
    reset_receiver();
    //std::cout << C_GREE << "Client created empty" << C_NONE << "\n";
}
Client::Client(int poll_fd_fd, ConfService& server_config, int index) : index(index)
{
	int	opt = 1;
    socklen_t addrlen = sizeof(client_addr);

	newClientSocket = accept(poll_fd_fd, (struct sockaddr*)&client_addr, &addrlen);
	if (newClientSocket == -1)
	{
		std::cout << "Accept error\n";
		throw std::runtime_error("Accept error");
	}


	setsockopt(newClientSocket, IPPROTO_TCP, SO_REUSEADDR, &opt, sizeof(opt));

	_poll_fd.fd = newClientSocket;
	_poll_fd.events = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	_poll_fd.revents = 0;
	_server_config = server_config;
    reset_receiver();
    //std::cout << C_GREE << "Client created" << C_NONE << "\n";
}

Client::~Client()
{
    //std::cout << C_RED << "Client destroyed" << C_NONE << "\n";
}


void	Client::reset_receiver()
{
	_request.clear();
	_request_received = false;
}

void	Client::clear()
{
	_request = "";
	responseBody = "";
}

int	Client::recieve_packet(int fd)
{
	// basically get_next_line
	char request_chunk[PACKAGE_SIZE];
	memset(request_chunk, 0, PACKAGE_SIZE);
	ssize_t bytes_read = recv(fd, request_chunk, PACKAGE_SIZE, MSG_DONTWAIT);
	if (bytes_read < 0) {
		_error = 500;
		return (-1);
	}
	if (bytes_read == 0 && _request.length() == 0) {
		return (0); // dead client lal remove from map und so
	}
	_request.append(request_chunk);
	if (bytes_read == 0 || strlen(request_chunk) < PACKAGE_SIZE - 1)
		_request_received = true;
	//std::cout << "received Packet " << bytes_read << "\n" << request_chunk << "\n";
	return (bytes_read);
}

int	Client::get_socket(){
	return (_poll_fd.fd);
}

struct pollfd&	Client::get_pollfd(){
	return (_poll_fd);
}

int	Client::get_error(){
	return (_error);
}

void	Client::set_error(int p_error){
	_error = p_error;
}

std::string	Client::get_request(){
	if (_request_received)
		return (_request);
	return ("");
}

bool Client::has_request()
{
	return _request_received;
}



void		Client::sendResponse()
{
	req.add(_request);
	std::cout << "Request :\n\n" << C_GRAY << _request << C_NONE << "\n";
	statusCode = 200;
	buildResponseBody();
	// std::cout << "get_path(): " << req.get_path() << std::endl;
    // std::cout << "get_path_o(): " << req.get_path_o() << std::endl;

    // // Content getters
    // std::cout << "get_body(): " << req.get_body() << std::endl;
    // std::cout << "get_method_enum(): " << req.get_method_enum() << std::endl;
    // std::cout << "get_method(): " << req.get_method() << std::endl;
    // std::cout << "get_version(): " << req.get_version() << std::endl;
    // std::cout << "get_header(\"example_key\"): " << req.get_header("example_key") << std::endl;
    // std::cout << "get_req(): " << req.get_request() << std::endl;
	// std::cout << req.get_path() << "\n";
	std::string  response ="";
	if(statusCode == 302)
	{
		response =  "HTTP/1.1 302 Found\r\n"
					"Location: " + redirectTo + "\r\n"
					"Content-Length: 0\r\n"
					"Connection: close\r\n"
					"\r\n";
		// std::cout << response << "\n";
	}
	else
	{
		response =
		    std::string("HTTP/1.1 ") + std::to_string(statusCode) + " " + httpStatusCodes[statusCode] + "\r\n"
			"Content-Type: text/html; charset=UTF-8\r\n" +
			"Content-Length: " + std::to_string(responseBody.length()) + "\r\n" +
			"\r\n" +
			responseBody;
	}
	char* c_response = new char[response.length() + 1];
	std::strcpy(c_response, response.c_str());
	// response = "hallo";
	// std::vector<char> c_response(response.begin(), response.end());
	// c_response.push_back('\0'); 
	if (sendto(newClientSocket, c_response, response.length(), 0,
               (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Fehler beim Senden der Antwort");
        // close(client_fd);
        return ;
    }
    // close(client_fd);
	// req.clear();
	std::cout << "response send 1\n";
}


std::string Client::choseRootPath(LocationRedirect *location)
{
	std::string rootPath = location->getRoot();
	if(rootPath == "")
	{
		rootPath = config.serverBlock[index].getRoot();
	}
	return rootPath;
}

std::string replacePath(std::string sbegin, const std::string& s1, const std::string& s2) {
    size_t pos = 0;
	size_t pos2 = 0;
	std::string slash = "";
	// std::cout << sbegin << "replacePathsbegin\n";
	if(s1.length() == 1)
	{
		return s2 + sbegin;
	}
	pos = sbegin.find(s1, pos);
	pos2 = sbegin.find(s1, pos) + s1.length();
	std::string result = sbegin.substr(0, pos) + s2 + sbegin.substr(pos2, sbegin.length() - pos2);
	// std::cout << result << "result\n";
    return result;
}

bool Client::iterateIndexFiles(std::string basicPath, std::vector<std::string> indexFiles)
{
	for (const auto& file : indexFiles)
	{
		std::ifstream indexFile(basicPath + file);
		if (!indexFile)
		{
			std::cout << "test7\n";
			continue;
		}
		responseBody = extractFile(indexFile);
		return true;
		// std::cout << responseBody << "\n";
	}
	return false;
}

void Client::loadErrorSite()
{
	std::cout << "error\n";
	if(_server_config.serverBlock[index].getErrorFilePaths()[statusCode] == "")
	{
		std::string defaultPath = "errorpagesdefault/" + std::to_string(statusCode) + ".html";
		std::ifstream errorFile(defaultPath);
		if(!errorFile)
		{
			responseBody = "errorpageunavailable . Error:" + std::to_string(statusCode);
			return;
		}
		responseBody = extractFile(errorFile);
	}
	else
	{
		std::string configPath = _server_config.serverBlock[index].getErrorFilePaths()[statusCode];
		std::cout << configPath << "\n";
		std::ifstream errorFile(configPath);
		if(!errorFile)
		{
			std::string defaultPath = "errorpagesdefault/" + std::to_string(statusCode) + ".html";
			std::ifstream errorFile(defaultPath);
			if(!errorFile)
			{
				responseBody = "errorpageunavailable . Error:" + std::to_string(statusCode);
				return;
			}
			responseBody = extractFile(errorFile);
			return;
		}
		responseBody = extractFile(errorFile);
		std::cout << responseBody << "\n";
	}
}

std::string parseBody(std::string request)
{
	std::vector<std::string> lines;
    std::istringstream iss(request);
    std::string line;
	std::cout << "parse Body" << "\n";
    // Request in Zeilen aufteilen
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }


	std::string::size_type pos = request.find("\r\n\r\n");
if (pos != std::string::npos) {
    std::string body = request.substr(pos + 4); // +4, um über das \r\n\r\n hinwegzugehen
    return body;
} else {
    std::cout << "Kein Body gefunden." << std::endl;
    return "";
}
	// std::string body = "";
    // // Body ausgeben (falls keine leere Zeile, nimm den gesamten String – aber im Beispiel gibt's eine)
    // if (body_start < lines.size()) {
    //     std::cout << "Extrahierter POST-Body:" << std::endl;
    //     for (size_t i = body_start; i < lines.size(); i++) {
    //         std::cout << lines[i] << std::endl;
	// 		body += lines[i];
	// 		body += "\r\n";

    //     }
	// 	std::cout << C_NONE;
	// 	std::cout << "parsed Body:\n" << body << "\n";
    // } else {
    //     std::cout << "Kein Body gefunden." << std::endl;
    // }
	// std::cout << "parsed Body:\n" << body << "\n";
    // return body;
}

bool Client::CGI(std::ifstream &responseFile, std::string finalPath)
{
	// Check if file is a CGI script
	std::string filePath = finalPath.substr(1, finalPath.length() - 1);
	// std::cout << filePath << "debug\n";
	CGIExecutor cgiExecutor;

	if (cgiExecutor.isCGIFile(filePath)) {
		std::cout << "Processing CGI file: " << filePath << std::endl;
		cgiExecutor.setContentType(req.get_content_type());
		// Extract query string from path
		std::string fullPath = req.get_path();
		std::string queryString = "";
		cgiExecutor.requestBody = parseBody(_request);
		if(req.get_method() == "GET")
		{
			size_t queryPos = fullPath.find('?');
			if (queryPos != std::string::npos) {
				queryString = fullPath.substr(queryPos + 1);
			}
		}
		else if (req.get_method() == "POST"){
				queryString = parseBody(_request);

				// return true;
		}

						// std::cout << "Query String: \n";
				// std::cout << queryString << "\n";
		// Create CGI request
		CGIRequest cgiRequest;
		cgiRequest.method = req.get_method();
		cgiRequest.scriptPath = filePath;
		cgiRequest.queryString = queryString;
		cgiRequest.body = req.get_body();
		
		// Set up headers
		// cgiRequest.headers["Content-Type"] = req.get_header("Content-Type");
		cgiRequest.headers["Host"] = req.get_header("Host");
		
		// Set up environment variables
		cgiRequest.env["SERVER_NAME"] = req.get_header("Host");
		cgiRequest.env["SERVER_PORT"] = "8019";
		cgiRequest.env["SCRIPT_NAME"] = req.get_path();
		cgiRequest.env["PATH_INFO"] = "";
		
		try {
			CGIResponse cgiResponse = cgiExecutor.execute(cgiRequest);
			if (cgiResponse.success) {
				responseBody = cgiResponse.body;
				statusCode = 200;
			} else {
				std::cerr << "CGI execution failed: " << cgiResponse.errorMessage << std::endl;
				statusCode = 500;
				responseBody = "Internal Server Error: CGI execution failed";
			}
		} catch (const std::exception& e) {
			std::cerr << "CGI execution error: " << e.what() << std::endl;
			statusCode = 500;
			responseBody = "Internal Server Error: CGI execution failed";
		}
		return true;
	} else {
		responseBody = extractFile(responseFile);
		return false;
	}
}

std::string redirect(std::string newPath)
{
std::string html1 = "<!DOCTYPE html><html><head><title>Testseite</title></head><body>";
std::string href = "<a href=\"http://localhost" + newPath + "\"></a>";
std::string html2 = "</body></html>";
std::cout << href << "\n";
return html1 + "\n" + href + "\n" + html2;
}

void		Client::buildResponseBody()
{
LocationRedirect *locationPointer;
LocationRedirect location;
std::string finalPath;
 std::cout << req.get_path() << " path\n";
std::string path = req.get_path();
std::string isRedirected = "no";
std::cout << "build response begin\n";
	std::cout << "redirect loop\n";
	std::cout << index << "\n";
	locationPointer = _server_config.serverBlock[index].getBestMatchingLocation(path);
	if(locationPointer)
	{
		std::cout << "location found\n";
		location = *locationPointer;
		std::string rootPath = choseRootPath(locationPointer);
		std::string locationUrl = location.getUrl();
		std::cout << locationUrl << "\n";
		std::cout << locationUrl.find_last_of('/') << "\n";
		finalPath = replacePath(req.get_path(), locationUrl, rootPath);
		std::vector<std::string> restrictedMethods = location.getRestrictedMethods();
		for(std::vector<std::string>::iterator it = restrictedMethods.begin(); it != restrictedMethods.end(); it ++)
		{
			if(*it.base() == req.get_method())
			{
				statusCode = 403;
				loadErrorSite();
				return;
			}
		}
		isRedirected = location.isRedirected();
		std::cout << isRedirected << " is redirected\n";
		if(isRedirected != "")
		{
			redirectTo = "http://localhost" + replacePath(req.get_path(), location.getUrl(), isRedirected);
			statusCode = 302;
			return;
		}

		std::cout << "location found\n";
		// std::ifstream responseFile("tmp/www");
		// std::cout << finalPath << "\n";
		finalPath = parameterSplit(finalPath)[0];
		if(req.get_method() == "DELETE")
		{
			if(access(finalPath.substr(1, finalPath.length() - 1).c_str(), F_OK))
			{
				statusCode = 404;
				loadErrorSite();
				return;
			}
			std::string removeString = "rm -f " + finalPath.substr(1, finalPath.length() - 1);
			std::cout << removeString << "\n";
			system(removeString.c_str());
			statusCode = 202;
			responseBody = "<h1>Successfully deleted";
			return;
		}
		// std::cout << finalPath << "\n";
		struct stat st;
		if (stat(finalPath.substr(1, finalPath.length() - 1).c_str(), &st) != 0) {
			if (errno == EACCES) {
				statusCode = 403;
			} else if (errno == ENOENT) {
				statusCode = 404;
			} else {
				statusCode = 500;
			}	
			loadErrorSite();
			return;
		}
		std::ifstream responseFile(finalPath.substr(1, finalPath.length() - 1));
		if (!responseFile)
		{
			// File exists but can't be opened (permission denied)
			statusCode = 403;
			loadErrorSite();
			return;
		}
		else if (std::filesystem::is_directory(finalPath.substr(1, finalPath.length() - 1)))
		{
			std::cout << "test5\n";
			std::vector<std::string> indexFiles = location.getIndexFiles();
			if(indexFiles.size() == 0)
			{
				std::cout << "test6\n";
				indexFiles = _server_config.serverBlock[index].getIndexFiles();
			}
			if(!iterateIndexFiles(finalPath.substr(1, finalPath.length() - 1) + "/", indexFiles))
			{
				if(location.getDirectoryListing())
				{
					// Directory listing - simplified for C++11 compatibility
					responseBody += "<h1>Directory Listing</h1>";
					responseBody += "<p>Directory listing functionality simplified</p>";
				}
				else
				{
					statusCode = 403;
					loadErrorSite();
				}
			}
			std::cout << "test6\n";
			return;
		}
		if(!CGI(responseFile, finalPath))
		{
			std::cout << "No CGI\n";
			std::ifstream file(finalPath.substr(1, finalPath.length() - 1));
			responseBody = extractFile(file);
		}
	}
	else
	{
		// No location matched - use default handling
		if(req.get_path() == "/")
		{
			std::cout << "standard index files\n";
			std::vector<std::string> indexFiles = _server_config.serverBlock[index].getIndexFiles();
			//try index files
			iterateIndexFiles("webcontent/", indexFiles);
		}
		else
		{
			// Path doesn't match any location and isn't root - 404
			statusCode = 404;
			loadErrorSite();
		}
	}
	std::cout << "request\n";

}




