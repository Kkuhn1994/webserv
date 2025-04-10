#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "LocationRedirect.hpp"

class ServerBlock
{
public:
    ServerBlock(const int port, const std::string serverName, const std::string host);
    ~ServerBlock();

private:
    const int port;
    const std::string serverName;
    const std::string host;
    std::map<int, std::string> pathOfErrorFiles;
    std::vector<LocationRedirect> location;
    std::string defaultFile;
};