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
#include <filesystem>

#include "ConfService.hpp"
#include "Request.hpp"
#include "utility.hpp"

#ifndef PACKAGE_SIZE
# define PACKAGE_SIZE 1024
#endif

class Client
{
	public:
		Client();
		Client(int const poll_fd_fd, class ConfService& server_config, int index);
		~Client(void);

		void			reset_receiver();
		int				recieve_packet(int fd);
        bool            has_request();

		int				get_socket();
		struct pollfd&	get_pollfd();
		int				get_error();
		void			set_error(int p_error);
		std::string		get_request();

		void			sendResponse();
		void			clear();
	private:
		int 					index;
		int 					newClientSocket;
		class ConfService		_server_config;
		class Request			req;
		class ConfService		config;
		struct pollfd			_poll_fd;
		int						_error;
		std::string				_request;
		bool					_request_received;
		struct sockaddr_in 		client_addr;
		std::string				responseBody;
		int						statusCode;
		std::string				redirectTo;

		void					buildResponseBody();
		std::string				choseRootPath(LocationRedirect *location);
		bool					iterateIndexFiles(std::string basicPath, std::vector<std::string> indexFiles);
		void 					loadErrorSite();

		bool CGI(std::ifstream &responseFile, std::string finalPath);

		std::map<int, std::string> httpStatusCodes = {
        {100, "Continue"},
        {101, "Switching Protocols"},
        {102, "Processing"},
        
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},
        
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {418, "I'm a teapot"}, // Easter egg ;-)
        
        {500, "Internal Server Error Custom"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"}
    };

};
