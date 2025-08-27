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

#define BUF_SIZE 1024
#define POLL_TIMEOUT 200

class WebServer
{
	public:
		WebServer(const std::string Path);
		~WebServer();

		void		openSockets();
		void		loopPollEvents();

	private:
		void		acceptRequest(int index);
		void		acceptClients(int server_fd);
		void		sendResponse(int client_fd);
		void		killClient(std::vector<struct pollfd>::iterator it);

		bool		deleteIfExists(std::string filename);
		bool		validateFilename(std::string filename);

		ConfService					config;
		std::string					config_path;
		std::vector<struct pollfd>	poll_fds;
		std::vector<int>			listening_socket_fds;
		std::map<int, Client>		fds_clients;
		int							_server;
};
