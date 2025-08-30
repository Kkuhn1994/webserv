#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "LocationRedirect.hpp"
#include <regex>
#include <fstream>
#include "utility.hpp"


class ServerBlock
{
public:
    ServerBlock(const int port, const std::string serverName, const std::string host);
    ~ServerBlock();
    ServerBlock& operator=(const ServerBlock&);

    void initialize(int blockNr);

    int getPort() const;
    std::string getRoot() const;
    std::vector<std::string> getIndexFiles();
    LocationRedirect *getBestMatchingLocation(std::string path);

private:
    const int port;
    const std::string serverName;
    const std::string host;
    std::string rootPath;

    std::vector<LocationRedirect> location;
    std::vector<std::string>        indexFiles;
    std::string defaultFile;
    std::map<int, std::string> pathOfErrorFiles;
    
    void initLocationRedirects(int blockNr);
    void initErrorPages(std::ifstream &serverFile);
    void initIndexFiles(std::ifstream &serverFile);

    std::string extractRoot(std::ifstream &serverFile);

};