#include "ConfService.hpp"
#include <iostream>
#include <fstream>
#include <string>

ConfService::ConfService()
{
    std::cout << "ConfService created\n";
}

ConfService::~ConfService()
{
    std::cout << "ConfService destroyed\n";
}

std::string ConfService::extractHostName(std::ifstream &serverFile)
{
	std::regex serverName(R"(server_name\s+([^\s;]+))");
	std::string line;
	while (std::getline(serverFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, serverName))
		{
			return match[1];
		}
	}
	return "";
}

std::string ConfService::extractServerName(std::ifstream &serverFile)
{
	std::regex serverName(R"(server_name\s+([^\s;]+))");
	std::string line;
	while (std::getline(serverFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, serverName))
		{
			return match[1];
		}
	}
	return "";
}

std::string ConfService::extractPort(std::ifstream &serverFile)
{
	std::regex ipv4Pattern(R"(listen\s+(\d+))");
	std::string line;
	while (std::getline(serverFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, ipv4Pattern))
		{
			return match[1];
		}
	}
	return "";
}

void ConfService::initialize()
{
    int i = 1;

    //create tempfiles
    deleteCommentsOfConfig();
    extractServerBlocks();
    extractLocationBlocks();
    //create serverclasses
    while (1)
	{

        std::string fileName = "conf/ServerConfig" + std::to_string(i) + ".txt";
		std::ifstream serverFile(fileName);
		if (!serverFile)
        {
        	break;
        }

		const std::string hostName = extractHostName(serverFile);
		serverFile.clear();
		serverFile.seekg(0, std::ios::beg);
		const std::string serverName = extractServerName(serverFile);
		serverFile.clear();
		serverFile.seekg(0, std::ios::beg);
		const std::string port = extractPort(serverFile);
		ServerBlock newBlock(std::stoi(port), serverName, hostName);
		newBlock.initialize(i);
		serverBlock.push_back(newBlock);
        i ++;
	}

}

std::vector<ServerBlock> ConfService::getServerBlocks()
{
    return serverBlock;
}

void ConfService::deleteCommentsOfConfig()
{
    std::ifstream file("conf/nginxconf");
    std::ofstream tempFile("conf/tempConfig.txt", std::ios::app);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.length() == 0 || *line.begin() == '#')
            continue;
        for (char ch : line)
        {
            if (ch == '#')
            {
                break;
            }
            tempFile << ch;
        }
        tempFile << '\n';
    }
    tempFile.close();
    file.close();
}

std::string ltrim(const std::string &s)
{
    size_t start = 0;
    while (start < s.length() && std::isspace(static_cast<unsigned char>(s[start])))
    {
        ++start;
    }
    return s.substr(start);
}

bool starts_with_server(const std::string &line)
{
    std::string trimmed = ltrim(line);
    return trimmed.rfind("server", 0) == 0; // prüft, ob "server" an Position 0 steht
}

bool starts_with_location(const std::string &line)
{
    std::string trimmed = ltrim(line);
    return trimmed.rfind("location", 0) == 0; // prüft, ob "location" an Position 0 steht
}

std::string extractBlock(std::ifstream &in, int brace_count)
{
    std::string result;
    char ch;

    while (in.get(ch))
    {
        if (ch == '{')
            brace_count++;
        result += ch;
        if (ch == '}')
        {
            brace_count--;
            if (brace_count == 0)
                break;
        }
    }
    return result;
}

void ConfService::extractServerBlocks()
{
    std::ifstream file("conf/tempConfig.txt");
    std::string line;
    int serverCount = 0;

    while (std::getline(file, line))
    {
        if (starts_with_server(line))
        {
            serverCount++;

            std::string fileName = "conf/ServerConfig" + std::to_string(serverCount) + ".txt";
            std::ofstream tempFile(fileName, std::ios::app);
            tempFile << line;
            tempFile << '\n';
            tempFile << extractBlock(file, 1);
            tempFile.close();
        }
    }
    file.close();
}

void ConfService::extractLocation(std::ifstream &file, int serverCount)
{
    std::string line;
    int locationCount = 0;

    while (std::getline(file, line))
    {
        if (starts_with_location(line))
        {
            locationCount++;
            std::string fileName = "conf/locationConfig_" + std::to_string(serverCount) + "_" + std::to_string(locationCount) + ".txt";
            std::ofstream tempFile(fileName, std::ios::app);
            tempFile << line;
            tempFile << '\n';
            tempFile << extractBlock(file, 1);
            tempFile.close();
        }
    }
    file.close();
}

void ConfService::extractLocationBlocks()
{
    int counter = 1;
    while (1)
    {
        std::string fileName = "conf/ServerConfig" + std::to_string(counter) + ".txt";
        std::ifstream file(fileName);
        if (!file)
            break;
        else
        {
            std::cout << fileName << std::endl;
            extractLocation(file, counter);
        }
        counter++;
    }
    std::cout << "\n";
}