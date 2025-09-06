#include "../include/Client.hpp"
#include <iostream>
#include <fstream>
#include <string>

Client::Client()
{
    
}
Client::Client(pollfd& poll_fd, ConfService& server_config)
{
	int newClientSocket;
	int	opt = 1;

	newClientSocket = accept(poll_fd.fd, NULL, NULL);
	if (newClientSocket == -1)
		throw std::runtime_error("Accept error");

	setsockopt(newClientSocket, IPPROTO_TCP, SO_REUSEADDR, &opt, sizeof(opt));

	_poll_fd.fd = newClientSocket;
	_poll_fd.events = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	_poll_fd.revents = 0;
	_server_config = server_config;
}

Client::~Client()
{
    std::cout << "Client destroyed\n";
}

int	Client::get_Socket(){
	return (_poll_fd.fd);
}

struct pollfd&	Client::get_pollfd(){
	return (_poll_fd);
}
