#include "../include/Server.hpp"

WebServer::WebServer(const std::string Path) : config_path(Path), full_request("")
{
	_server = socket(AF_INET, SOCK_STREAM, 0);
    if (_server < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
	config.initialize(Path);

	std::cout << "LAL\n";
}

WebServer::~WebServer()
{
	poll_fds.clear();
}

void WebServer::openSockets()
{
	int	index = 0;
	std::vector<int> usedPorts; // CGI ADDITION: Track used ports to avoid duplicates (workaround for config parser bug)
	
	for (std::vector<ServerBlock>::iterator it = config.serverBlock.begin(); it != config.serverBlock.end(); it++)
	{
		int port = it.base()->getPort();
		
		// CGI ADDITION: Skip if port already used (prevents binding errors from duplicate configs)
		if (std::find(usedPorts.begin(), usedPorts.end(), port) != usedPorts.end()) {
			std::cout << "Skipping duplicate port: " << port << std::endl;
			continue;
		}
		usedPorts.push_back(port);
		
		struct pollfd listening_poll;
		struct protoent		*prtdb;
		int					opt = 1;
		struct sockaddr_in	_address;

		_address.sin_family = AF_INET;
	
		_address.sin_addr.s_addr = INADDR_ANY;
		_address.sin_port = htons(it.base()->getPort());
		prtdb = getprotobyname("TCP");

		listening_poll.fd = socket(PF_INET, SOCK_STREAM, prtdb->p_proto);
		if (listening_poll.fd < 0)
			throw std::runtime_error("socket: Error creating server socket");
		#ifdef __APPLE__
		if (fcntl(listening_poll.fd, F_SETFL, O_NONBLOCK) < 0)  // CGI ADDITION: Fixed - was using wrong fd
			throw std::runtime_error("Error setting server socket to non-blocking");
		#endif
		int optreturn = setsockopt(listening_poll.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  // CGI ADDITION: Fixed socket bug - was using this->_server instead of listening_poll.fd
		std::cout << optreturn << "\n";
		if (optreturn == -1) {
			perror("setsockopt failed");
		}
		if (optreturn != 0)
			throw std::runtime_error("Could not set socket options");
		if (bind(listening_poll.fd, (struct sockaddr*)&(_address), sizeof(_address)) < 0)
			throw std::runtime_error("Error binding server socket port");
		int listenreturn = listen(listening_poll.fd, SOMAXCONN); // why 3
		if (listenreturn < 0)
			throw std::runtime_error("Could not listen");
		listening_poll.events = POLLIN;
		listening_poll.revents = 0;
		poll_fds.push_back(listening_poll);

		std::cout << "Socket " << (index++) << " (FD " << listening_poll.fd
				  << ") is listening on: " << "edit host here" << ":" << it.base()->getPort() << std::endl;
	}
}

void WebServer::loopPollEvents()
{
	while(1)
	{
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0)
        {
            throw std::runtime_error("Error with poll()");
        }
		for (size_t index = 0; index < poll_fds.size(); index ++)
		{
			if (poll_fds[index].revents & POLLIN) {
				acceptRequest(index);
				if (connection_valid) {
					sendResponse(index);
				}
			} else if (poll_fds[index].revents & POLLERR) {
				std::cout << "Socket error occurred.\n";
			} else if (poll_fds[index].revents & POLLHUP) {
				std::cout << "Socket hung up (client disconnected).\n";
			}
		}
	}
}

void		WebServer::acceptRequest(int index)
{
    connection_valid = false;
    socklen_t addrlen = sizeof(client_addr);
    
    client_fd = accept(poll_fds[index].fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        return;
    }
    char buffer[1024];
	full_request = "";
    
    ssize_t bytes_received;
    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        full_request.append(buffer, bytes_received);
        if (full_request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (full_request.empty()) {
                close(client_fd);
                return;
            }
        } else {
            perror("recv failed");
            close(client_fd);
            return;
        }
    }
    
    if (full_request.empty() || full_request.find("\r\n\r\n") == std::string::npos) {
        close(client_fd);
        return;
    }
    
    connection_valid = true;
}

void		WebServer::sendResponse(int index)
{
	std::cout << full_request << "\n";
	req.add(full_request);
	buildResponseBody(index);
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
	if (send(client_fd, response.c_str(), response.length(), 0) < 0) {
        perror("Fehler beim Senden der Antwort");
        close(client_fd);
        return ;
    }
    close(client_fd);
	req.clear();
}

std::string WebServer::choseRootPath(int index, LocationRedirect *location)
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

void WebServer::iterateIndexFiles(std::string basicPath, std::vector<std::string> indexFiles)
{
	for (const auto& file : indexFiles)
	{
		std::ifstream indexFile(basicPath + file);
		if (!indexFile)
		{
			continue;
		}
		responseBody = extractFile(indexFile);
		std::cout << responseBody << "\n";
	}
}

void		WebServer::buildResponseBody(int index)
{
	LocationRedirect *location;
	std::string finalPath;
	std::string path = req.get_path();
	std::string isRedirected = "INITIAL";
	int loopCount = 0;
	while(isRedirected != "" && loopCount < 10)  // Add safety counter
	{
		loopCount++;
		location = config.serverBlock[index].getBestMatchingLocation(path);
		if(location)
		{
			std::string rootPath = choseRootPath(index, location);
			std::string locationUrl = location->getUrl();
			finalPath = replacePath(req.get_path(), locationUrl, rootPath);
			
			// CGI file check - skip redirects if valid
			std::string testCleanPath = finalPath.substr(1, finalPath.length() - 1);
			size_t queryPos = testCleanPath.find('?');
			std::string filePathOnly = (queryPos != std::string::npos) ? testCleanPath.substr(0, queryPos) : testCleanPath;
			
			if (CGIExecutor::isCGIFile(filePathOnly)) {
				std::ifstream testFile(filePathOnly);
				if (testFile.good()) {
					testFile.close();
					finalPath = "/" + filePathOnly;
					break; 
				}
				testFile.close();
			}
			
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
			path = isRedirected;
		}
	}
	if(location)
	{
		std::string cleanPath = finalPath.substr(1, finalPath.length() - 1);
		
		// Strip query string for file access
		size_t queryPos = cleanPath.find('?');
		if (queryPos != std::string::npos) {
			cleanPath = cleanPath.substr(0, queryPos);
		}
		
		// CGI check
		if (CGIExecutor::isCGIFile(cleanPath)) {
			CGIExecutor executor;
			CGIRequest cgiReq;
			cgiReq.method = req.get_method();
			cgiReq.scriptPath = cleanPath;
			
			// Parse query string
			std::string fullPath = req.get_path();
			size_t queryPos = fullPath.find('?');
			if (queryPos != std::string::npos) {
				cgiReq.queryString = fullPath.substr(queryPos + 1);
			} else {
				cgiReq.queryString = "";
			}
			
			cgiReq.body = req.get_body();
			cgiReq.headers["Content-Type"] = req.get_header("Content-Type");
			
			// Add environment variables from location config
			std::map<std::string, std::string> params = location->getCGIParams();
			for (const auto& param : params) {
				cgiReq.env[param.first] = param.second;
			}
			
			CGIResponse cgiResp = executor.execute(cgiReq);
			
			if (cgiResp.success) {
				responseBody = cgiResp.body;
				return;
			} else {
				statusCode = 500;
				responseBody = cgiResp.body;
				return;
			}
		}
		
		std::ifstream responseFile("tmp/www");
		// std::ifstream responseFile(finalPath.substr(1, finalPath.length() - 1));
		if (!responseFile)
		{
			statusCode = 404;
			return;
		}
		
		struct stat statBuf;
		if (stat(cleanPath.c_str(), &statBuf) == 0 && S_ISDIR(statBuf.st_mode))
		{
			std::cout << "test5\n";
			std::vector<std::string> indexFiles = location->getIndexFiles();
			if(indexFiles.size() == 0)
			{
				indexFiles = config.serverBlock[index].getIndexFiles();
			}
			iterateIndexFiles(finalPath.substr(1, finalPath.length() - 1) + "/", indexFiles);
			return;
		}
		std::cout << "test6\n";
		responseBody = extractFile(responseFile);
		
	}
	if(req.get_path() == "/")
	{
		std::vector<std::string> indexFiles = config.serverBlock[index].getIndexFiles();
		//try index files
		iterateIndexFiles("webcontent/", indexFiles);
	}


}


// CGI ADDITION: Helper function to check if a string ends with a given suffix
bool endsWith(const std::string& str, const std::string& suffix) {
	if (str.length() < suffix.length()) return false;
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// CGI ADDITION: Simple function to check if the request is for a CGI script
bool isCGIRequest(const std::string& path) {
	return endsWith(path, ".php") || endsWith(path, ".py") || endsWith(path, ".pl");
}
