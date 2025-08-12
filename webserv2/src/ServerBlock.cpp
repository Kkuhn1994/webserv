#include "ServerBlock.hpp"

ServerBlock::ServerBlock(const int _port, const std::string _serverName,
                         const std::string _host)
    : port(_port), serverName(_serverName), host(_host)
{
    std::cout << "ServerBlock created\n";
    std::cout << "Port: " << port << "\n";
    std::cout << "Server Name: " << serverName << "\n";
    std::cout << "Host: " << host << "\n";
    std::cout << "\n";
}

ServerBlock::~ServerBlock()
{
    std::cout << "ServerBlock destroyed\n";
}

void ServerBlock::initLocationRedirects(int blockNr)
{
	int i = 0;
	while (++i)
	{
		std::string fileName = "conf/locationConfig_" + std::to_string(blockNr) + "_" + std::to_string(i) + ".txt";
		std::ifstream testFile(fileName);
		if (!testFile.good())
		{
			break;
		}
		testFile.close();
		std::ifstream locationRedirectFile(fileName);
		if (!locationRedirectFile)
			break;
		LocationRedirect *redirect = new LocationRedirect(locationRedirectFile);
		location.push_back(redirect);
	}
}

void ServerBlock::initialize(int blockNr) {
	initLocationRedirects(blockNr);

	std::string fileName = "../conf/ServerConfig" + std::to_string(blockNr) + ".txt";
	std::ifstream serverFile(fileName);
	if (!serverFile)
		return;
}