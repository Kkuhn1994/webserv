#pragma once

#include <poll.h>
#include <vector>
#include <iostream>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>
#include <map>
#include <set>
#include <sstream>
#include <netdb.h>



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
		void		openSockets();
		void		loopPollEvents();

	private:
		void		acceptRequest(int index);
	//	void		acceptClients(int server_fd);
		void		sendResponse(int index);
		void		killClient(std::vector<struct pollfd>::iterator it);

		bool		deleteIfExists(std::string filename);
		bool		validateFilename(std::string filename);
		void		buildResponseBody(int index);

		ConfService					config;
		Request						req;
	

		std::string					config_path;
		std::string					full_request;
		std::vector<struct pollfd>	poll_fds;
		std::vector<int>			listening_socket_fds;
		std::map<int, Client>		fds_clients;
		int							_server;
		int 						client_fd;
		struct sockaddr_in 			client_addr;
		std::string					responseBody;
};
