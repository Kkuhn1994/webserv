#include "../include/Server.hpp"

WebServer::WebServer(const std::string Path) : config_path(Path)
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
		std::cout << htons(80) << "\n\n";
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
			} else if (poll_fds[index].revents & POLLERR) {
				std::cout << "Socket error occurred.\n";
			} else if (poll_fds[index].revents & POLLHUP) {
				std::cout << "Socket hung up (client disconnected).\n";
			}
		}
	}
}

void	WebServer::acceptRequest(int index)
{
	std::cout << "Request received (POLLIN)\n";
   	struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    
    // Der Server akzeptiert die Verbindung und erhält den Client-Socket
    int client_fd = accept(poll_fds[index].fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        return;
    }
    char buffer[1024];
	std::string full_request = "";
    
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

    // Ausgabe des empfangenen Requests
    std::cout << "Received request: " << full_request << std::endl;

    // Null-terminate the received data to print it as a string (optional)
    buffer[bytes_received] = '\0';
    
    // Ausgabe des empfangenen Requests (optional)
    std::cout << "Received request: " << buffer << std::endl;

    // Schließe den Client-Socket, wenn die Kommunikation abgeschlossen ist
    close(client_fd);
}