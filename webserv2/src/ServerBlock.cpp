#include "../include/ServerBlock.hpp"

ServerBlock::ServerBlock(const int _port, const std::string _serverName,
                         const std::string _host) : port(_port), serverName(_serverName),
                                                    host(_host)
{
    std::cout << "ServerBlock created\n";
}

ServerBlock::~ServerBlock()
{
    std::cout << "ServerBlock destroyed\n";
}

int ServerBlock::getPort() const
{
    return (port); 
}