#include "ServerBlock.hpp"

ServerBlock::ServerBlock(const int _port, const std::string _serverName,
                         const std::string _host)
    : port(_port), serverName(_serverName), host(_host)
{
    std::cout << "\n";  
	std::cout << "ServerBlock created\n";
    std::cout << "Port: " << port << "\n";
    std::cout << "Server Name: " << serverName << "\n";
    std::cout << "Host: " << host << "\n"; 
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

void ServerBlock::initialize(int blockNr) 
{
	
	std::string fileName = "conf/ServerConfig" + std::to_string(blockNr) + ".txt";
	std::ifstream serverFile(fileName);
	if (!serverFile)
		return;
	initLocationRedirects(blockNr);
	initErrorPages(serverFile);
	serverFile.clear();
	serverFile.seekg(0, std::ios::beg);
	for (const auto& pair : pathOfErrorFiles) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
	std::cout << std::endl;
}

void ServerBlock::initErrorPages(std::ifstream &serverFile) 
{
	std::regex pattern(R"(error_page\s+([^;]+))");
	std::string line;
	std::vector<std::string> errorPageLine;
	std::string pathToPage;
	std::string pathToPageNew;
	while (std::getline(serverFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, pattern))
		{
			errorPageLine = split(match[1]);
			pathToPage = errorPageLine.back();
			errorPageLine.pop_back();
			for (const std::string& errorCode : errorPageLine) {
				pathToPageNew = resolveWildcardErrorCode(errorCode, pathToPage);
				pathOfErrorFiles.insert(std::make_pair(std::stoi(errorCode), pathToPageNew));
			}
		}
	}
}