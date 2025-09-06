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

class Client
{
	public:
		Client();
		Client(struct pollfd& poll_fd, class ConfService& server_config);
		~Client(void);

		int				get_Socket();
		struct pollfd&	get_pollfd();
	private:
		class ConfService		_server_config;
		struct pollfd			_poll_fd;
};
