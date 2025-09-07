#include "../include/ServerBlock.hpp"

ServerBlock::ServerBlock(const int _port, const std::string _serverName,
                         const std::string _host)
    : port(_port), serverName(_serverName), host(_host), rootPath("")
{
    std::cout << "\n";  
	std::cout << C_GREE << "ServerBlock created" << C_NONE << "\n";
    std::cout << "Port: " << port << "\n";
    std::cout << "Server Name: " << serverName << "\n";
    std::cout << "Host: " << host << "\n"; 
}

ServerBlock::~ServerBlock()
{
    std::cout << C_RED << "ServerBlock destroyed" << C_NONE << "\n";
}

int ServerBlock::getPort() const
{
	return port;
}

ServerBlock& ServerBlock::operator=(const ServerBlock&)
{
	return *this;
}

void ServerBlock::initLocationRedirects(int blockNr)
{
	std::string fileName1 = "conf/ServerConfig" + std::to_string(blockNr) + ".txt";
	int i = 0;
	while (++i)
	{
		std::string fileName2 = "conf/locationConfig_" + std::to_string(blockNr) + "_" + std::to_string(i) + ".txt";
		std::ifstream locationRedirectFile(fileName2);
		if (!locationRedirectFile)
			break;
		LocationRedirect redirect;

		deleteBlock(fileName1, fileName2, location.size());
		redirect.initialize(blockNr, i, locationRedirectFile);
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
	serverFile.clear();
	serverFile.seekg(0, std::ios::beg);
	initErrorPages(serverFile);
	serverFile.clear();
	serverFile.seekg(0, std::ios::beg);
	initIndexFiles(serverFile);
	serverFile.clear();
	serverFile.seekg(0, std::ios::beg);
	rootPath = extractRoot(serverFile);
	serverFile.clear();
	serverFile.seekg(0, std::ios::beg);
	// for (const auto& pair : pathOfErrorFiles) {
    //     std::cout << pair.first << " : " << pair.second << std::endl;
    // }
	// std::cout << std::endl;
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

void ServerBlock::initIndexFiles(std::ifstream &serverFile)
{
	std::regex pattern(R"(index\s+([^;]+))");
	std::string line;
	while (std::getline(serverFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, pattern))
		{
			std::cout << match[1];
			indexFiles = split(match[1]);
			return;
		}
	}
}

std::vector<std::string> ServerBlock::getIndexFiles()
{
	return indexFiles;
}

int pathMatch(std::string path1, std::string path2)
{
	int pathMatch = 1;
	int index = 0;
	std::vector<std::string> path1Parts = pathSplit(path1);
	std::vector<std::string> path2Parts = pathSplit(path2);
	for (std::vector<std::string>::iterator it = path1Parts.begin(); it != path1Parts.end(); it++)
	{
		// std::cout << path2Parts[index] << "\n";
		if(index == path2Parts.size())
		{
			break;
		}
		if(*it.base() != path2Parts[index])
		{
			return pathMatch;
		}
		pathMatch += path2Parts[index].length();
		index ++;
	}
	return pathMatch - 1;
}

LocationRedirect *ServerBlock::getBestMatchingLocation(std::string path)
{
	int highest_match = 0;
	int highest_match_new = 0;
	int highest_match_index = 0;
	int index = 0;
	
	for (std::vector<LocationRedirect>::iterator it = location.begin(); it != location.end(); it++)
	{
		std::string locationPath = it.base()->getUrl();
		highest_match_new = pathMatch(path, locationPath);
		if(highest_match_new > highest_match)
		{

			if(highest_match_new == 1 && locationPath.length() == 1 || highest_match_new > 1)
			{
				highest_match = highest_match_new;
				highest_match_index = index;
			}

		}
		index ++;
	}
	if(highest_match != 0)
	{
		std::cout << location[highest_match_index].getUrl() << "\n\n";
		return &location[highest_match_index];
	}
	return NULL;
}

std::string ServerBlock::extractRoot(std::ifstream &locationFile)
{
	std::regex locationRegex(R"(root\s+(?:~|=\s*)?([^\s{;]+))");
	std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, locationRegex))
		{
			return match[1];
		}
	}
	return "";
}

std::string ServerBlock::getRoot() const
{
	return rootPath;
}
