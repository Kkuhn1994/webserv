#pragma once

#include <sys/types.h>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <time.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <poll.h>
#include <netdb.h>

#include "ConfService.hpp"
#include "utility.hpp"

#ifndef PACKAGE_SIZE
# define PACKAGE_SIZE 1024
#endif

class Client
{
	public:
		Client();
		Client(int const poll_fd_fd, class ConfService& server_config);
		~Client(void);

		void			reset_receiver();
		int				recieve_packet(int fd);

		int				get_socket();
		struct pollfd&	get_pollfd();
		int				get_error();
		void			set_error(int p_error);
		std::string		get_request();
	private:
		class ConfService		_server_config;
		struct pollfd			_poll_fd;
		int						_error;
		std::string				_request;
		bool					_request_received;
		struct sockaddr_in 		client_addr;
};
