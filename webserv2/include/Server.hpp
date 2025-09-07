#pragma once

#include <poll.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>
#include <map>
#include <set>
#include <netdb.h>
#include <filesystem>



#include "ConfService.hpp"
#include "Client.hpp"
#include "Request.hpp"


#define BUF_SIZE 1024
#define POLL_TIMEOUT 200


class WebServer
{
	public:
		WebServer(const std::string Path);
		~WebServer();
		// Request						req;

		void		openServerSockets();
		void		loopPollEvents();
	private:
		void		acceptRequest(int index);
	//	void		acceptClients(int server_fd);
		void		sendResponse(int index);
		void		killClient(std::vector<struct pollfd>::iterator it);

		bool		deleteIfExists(std::string filename);
		bool		validateFilename(std::string filename);
		void		buildResponseBody(int index);

		std::string choseRootPath(int index, LocationRedirect *location);
		void iterateIndexFiles(std::string basicPath, std::vector<std::string> indexFiles);

		ConfService					config;
		Request						req;
	

		std::string					config_path;
		std::string					full_request;
		std::vector<struct pollfd>	poll_fds;
		std::vector<int>			listening_socket_fds;
		std::map<int, Client>		_clients;
		int							_server;
		int							_n_server;
		int 						client_fd;
		int							statusCode;
		struct sockaddr_in 			client_addr;
		std::string					responseBody;
};
