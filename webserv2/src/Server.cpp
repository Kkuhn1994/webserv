#include "../include/Server.hpp"

WebServer::WebServer(const std::string Path) : config_path(Path), full_request("")
{
	// config = ConfService(this->config_path);
	_server = socket(AF_INET, SOCK_STREAM, 0);
    if (_server < 0) {
        perror("socket failed");
        return ;
    }
	config.initialize(Path);

	std::cout << "LAL\n";
}

WebServer::~WebServer()
{}

void WebServer::openSockets()
{
	int	index = 0;
	for (std::vector<ServerBlock>::iterator it = config.serverBlock.begin(); it != config.serverBlock.end(); it++)
	{
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
		// if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
		// 	throw std::runtime_error("Error setting server socket to non-blocking");
		#endif
		int optreturn = setsockopt(this->_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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
				sendResponse(index);
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
	std::cout << "Request received (POLLIN)\n";
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
            break; // Anfrage ist wahrscheinlich vollständig
        }
    }
    if (bytes_received < 0) {
        perror("recv failed");
        close(client_fd);
        return;
    }
	std::cout << "test2\n";
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
	char* c_response = new char[response.length() + 1];
	std::strcpy(c_response, response.c_str());
	if (sendto(client_fd, c_response, response.length(), 0, 
               (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Fehler beim Senden der Antwort");
        close(client_fd);
        return ;
    }
    close(client_fd);
	req.clear();
}



void		WebServer::buildResponseBody(int index)
{
	std::cout << req.get_path() << " path\n";
	LocationRedirect *location = config.serverBlock[index].getBestMatchingLocation(req.get_path());
	// if(req.get_path() == "/")
	// {
		std::vector<std::string> indexFiles = config.serverBlock[index].getIndexFiles();
		//try index files
        for (const auto& file : indexFiles)
        {
			std::ifstream indexFile("webcontent/" + file);
			if (!indexFile)
			{
				continue;
			}
			responseBody = extractFile(indexFile);
        }
	// }
}



