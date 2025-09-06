#include "../include/Client.hpp"
#include <iostream>
#include <fstream>
#include <string>

Client::Client()
{
    reset_receiver();
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
    reset_receiver();
}

Client::~Client()
{
    std::cout << "Client destroyed\n";
}


void	Client::reset_receiver()
{
	_request.clear();
	_request_received = false;
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
	for (size_t size = 0; size < strlen(request_chunk); size++)
        _request.push_back(request_chunk[size]);
	if (bytes_read == 0 || strlen(request_chunk) < PACKAGE_SIZE)
		_request_received = true;
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

