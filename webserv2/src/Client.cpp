#include "../include/Client.hpp"
#include <iostream>
#include <fstream>
#include <string>

Client::Client()
{
    reset_receiver();
    std::cout << C_GREE << "Client created empty" << C_NONE << "\n";
}
Client::Client(int poll_fd_fd, ConfService& server_config, int index) : index(index)
{
	int	opt = 1;
    socklen_t addrlen = sizeof(client_addr);

	newClientSocket = accept(poll_fd_fd, (struct sockaddr*)&client_addr, &addrlen);
	if (newClientSocket == -1)
		throw std::runtime_error("Accept error");

	setsockopt(newClientSocket, IPPROTO_TCP, SO_REUSEADDR, &opt, sizeof(opt));

	_poll_fd.fd = newClientSocket;
	_poll_fd.events = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	_poll_fd.revents = 0;
	_server_config = server_config;
    reset_receiver();
    std::cout << C_GREE << "Client created" << C_NONE << "\n";
}

Client::~Client()
{
    std::cout << C_RED << "Client destroyed" << C_NONE << "\n";
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
	if (bytes_read == 0 || strlen(request_chunk) < PACKAGE_SIZE)
		_request_received = true;
	std::cout << "received Packet " << bytes_read << "\n";
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

void		Client::sendResponse()
{
	req.add(_request);
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

	std::string  response = 
		    std::string("HTTP/1.1 200 OK\r\n") +
			"Content-Type: text/html; charset=UTF-8\r\n" +
			"Content-Length: " + std::to_string(responseBody.length()) + "\r\n" +
			"\r\n" +
			responseBody;
	char* c_response = new char[response.length() + 1];
	std::strcpy(c_response, response.c_str());
	if (sendto(newClientSocket, c_response, response.length(), 0, 
               (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Fehler beim Senden der Antwort");
        // close(client_fd);
        return ;
    }
    // close(client_fd); !!! fds closen
	req.clear();
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
	std::string slash = "";
	if(s1.length() == 1)
	{
		slash = "/";
	} 
    while ((pos = sbegin.find(s1, pos)) != std::string::npos) {
        sbegin.replace(pos, s1.length(), s2 + slash);
        pos += s2.length() + slash.length(); // Move past the replacement to avoid infinite loops
	}
    return sbegin;
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

void		Client::buildResponseBody()
{
	LocationRedirect *location;
	std::string finalPath;
	// std::cout << req.get_path() << " path\n";
	std::string path = req.get_path();
	std::string isRedirected = "no";
	while(isRedirected != "")
	{
		location = _server_config.serverBlock[index].getBestMatchingLocation(path);
		if(location)
		{
			std::string rootPath = choseRootPath(location);
			std::string locationUrl = location->getUrl();
			finalPath = replacePath(req.get_path(), locationUrl, rootPath);
			std::vector<std::string> restrictedMethods = location->getRestrictedMethods();
			for(std::vector<std::string>::iterator it = restrictedMethods.begin(); it != restrictedMethods.end(); it ++)
			{
				if(*it.base() == req.get_method())
				{
					statusCode = location->getStatusCode();
					responseBody = location->getMessage();
					return;
				}
			}
			isRedirected = location->isRedirected();
			// std::cout << isRedirected << "\n";
			path = isRedirected;
		}
	}
	if(location)
	{
		std::ifstream responseFile("tmp/www");
		// std::ifstream responseFile(finalPath.substr(1, finalPath.length() - 1));
		if (!responseFile)
		{
			statusCode = 404;
			return;
		}
		else if (std::filesystem::is_directory(finalPath.substr(1, finalPath.length() - 1)))
		{
			std::cout << "test5\n";
			std::vector<std::string> indexFiles = location->getIndexFiles();
			if(indexFiles.size() == 0)
			{
				std::cout << "test6\n";
				indexFiles = _server_config.serverBlock[index].getIndexFiles();
			}
			if(!iterateIndexFiles(finalPath.substr(1, finalPath.length() - 1) + "/", indexFiles))
			{
				if(location->getDirectoryListing())
				{
					for (const auto& entry : std::filesystem::directory_iterator(finalPath.substr(1, finalPath.length() - 1))) {
						responseBody += entry.path().filename().string() + "<br>";
					}
				}
				else
				{
					statusCode = 403;
				}

			}
			return;
		}
		std::cout << "test6\n";
		responseBody = extractFile(responseFile);	
	}
	if(req.get_path() == "/")
	{
		std::vector<std::string> indexFiles = _server_config.serverBlock[index].getIndexFiles();
		//try index files
		iterateIndexFiles("webcontent/", indexFiles);
	}
}


