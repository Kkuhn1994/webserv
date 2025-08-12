#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "LocationRedirect.hpp"
#include <regex>
#include <fstream>


class ServerBlock
{
public:
    ServerBlock(const int port, const std::string serverName, const std::string host);
    ~ServerBlock();

    void initialize(int blockNr);

private:
    const int port;
    const std::string serverName;
    const std::string host;
    std::map<int, std::string> pathOfErrorFiles;
    std::vector<LocationRedirect *> location;
    std::string defaultFile;
    
    void initLocationRedirects(int blockNr);
};