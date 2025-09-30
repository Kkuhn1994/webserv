#include "../include/Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

void dropPrivileges(const char* username) {
    struct passwd* pw = getpwnam(username);
    if (!pw) {
        std::cerr << "Benutzer '" << username << "' nicht gefunden." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (setgid(pw->pw_gid) != 0) {
        perror("setgid fehlgeschlagen");
        exit(EXIT_FAILURE);
    }

    if (setuid(pw->pw_uid) != 0) {
        perror("setuid fehlgeschlagen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Rechte erfolgreich abgegeben auf Benutzer " << username
              << " (UID=" << pw->pw_uid << ", GID=" << pw->pw_gid << ")" << std::endl;
}


std::vector<struct pollfd>* g_poll_fds_ptr = nullptr;

WebServer::WebServer(const std::string Path) : config_path(Path), full_request("")
{
	_server = socket(AF_INET, SOCK_STREAM, 0);
    if (_server < 0) {
        perror("socket failed");
        return ;
    }
	config.initialize(Path);

}

WebServer::~WebServer()
{
	poll_fds.clear();
}

void handle_sigint(int) {
	std::cout << "\nSIGINT empfangen. Server wird heruntergefahren...\n";
	if (g_poll_fds_ptr) {
		for (auto& pfd : *g_poll_fds_ptr) {
			std::cout << "Schließe Socket FD: " << pfd.fd << "\n";
			close(pfd.fd);
		}
	}
	std::exit(0);
}
void WebServer::openServerSockets() // all these throw statements should be cleaned for potential leaks
{
	_n_server = 0;
	g_poll_fds_ptr = &poll_fds;

	std::signal(SIGINT, handle_sigint);
	
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
		if (fcntl(listening_poll.fd, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Error setting server socket to non-blocking");
		#endif
		int optreturn = setsockopt(listening_poll.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		std::cout << optreturn << "\n";
		if (optreturn == -1) {
			throw std::runtime_error("setsockopt failed");
		}
		if (optreturn != 0)
			throw std::runtime_error("Could not set socket options");
		if (bind(listening_poll.fd, (struct sockaddr*)&(_address), sizeof(_address)) < 0)
			throw std::runtime_error("Error binding server socket port");
		int listenreturn = listen(listening_poll.fd, SOMAXCONN);
		if (listenreturn < 0)
			throw std::runtime_error("Could not listen");
		dropPrivileges("kkuhn");
		listening_poll.events = POLLIN;
		listening_poll.revents = 0;
		poll_fds.push_back(listening_poll);

		_n_server++;
		std::cout << "Socket " << _n_server << " (FD " << listening_poll.fd
				  << ") is listening on: " << "edit host here" << ":" << it.base()->getPort() << std::endl;
	}
}

void WebServer::loopPollEvents()
{
	int index = 0;
	while(1)
	{
		// std::cout << "loop\n";
		// add shutdown check, implemented to also be called from destructor
		int poll_count = poll(poll_fds.data(), poll_fds.size(), 1000); // we should probably not give a negative timeout for poll
        if (poll_count < 0)
			continue ;
		index = 0;
		for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); it++)
		{
			if (it->revents & POLLIN) {
				// std::cout << "in\n";
				if (it < poll_fds.begin() + _n_server)
				{
					// std::cout << "new\n";
					// std::cout << "received new client stored at " << it->fd << "\n";
					Client c(it->fd, config, index);
					// std::cout << "original " << c.get_socket() << "\n";
					this->_clients.insert(std::make_pair(c.get_socket(), c));
					// std::cout << "received new client stored at " << it->fd << " its fd is " << c.get_socket() << "\n";
					poll_fds.push_back(c.get_pollfd());
					break ;
				}
				else
				{
					std::cout << "old\n";
					this->_clients[it->fd].recieve_packet(it->fd);
					if (this->_clients[it->fd].get_request().length() > 0)
						this->_clients[it->fd].get_request();

					this->_clients[it->fd].sendResponse();
					// this->_clients[it->fd].clear();
					close(poll_fds.back().fd);
					poll_fds.pop_back();
					this->_clients.erase(it->fd);
					std::cout << "response send\n";
					break;		
				}
			} else if (it->revents & POLLERR) {
				std::cout << "Socket error occurred.\n";
			} else if (it->revents & POLLHUP) {
				std::cout << "Socket hung up (client disconnected).\n";
			}
			index ++;
		}
	}
}
