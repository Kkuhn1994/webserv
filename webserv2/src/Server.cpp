#include "../include/Server.hpp"

WebServer::WebServer(const std::string Path) : config_path(Path)
{
	// config = ConfService(this->config_path);
	_server = 3;
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
		std::cout << htons(it->getPort()) << "\n\n";
		_address.sin_port = htons(it.base()->getPort());
		prtdb = getprotobyname("TCP");

		listening_poll.fd = socket(PF_INET, SOCK_STREAM, prtdb->p_proto);
		if (listening_poll.fd < 0)
			throw std::runtime_error("socket: Error creating server socket");
		#ifdef __APPLE__
		if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Error setting server socket to non-blocking");
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
		int listenreturn = listen(this->_server, 3); // why 3
		if (listenreturn < 0)
			throw std::runtime_error("Could not listen");
		listening_poll.events = POLLIN;
		listening_poll.revents = 0;
		poll_fds.push_back(listening_poll);

		std::cout << "Socket " << (index++) << " (FD " << listening_poll.fd
				  << ") is listening on: " << "edit host here" << ":" << it.base()->getPort() << std::endl;
	}
}