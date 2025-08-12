#include "../include/webserv.hpp"

WebServer::WebServer(std::string path): config_path(path)
{
	this.config = 
	listening_port_no = check.checkConfig(config_path);
}

WebServer::~WebServer()
{}