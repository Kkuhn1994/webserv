#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>


class LocationRedirect
{
public:
    LocationRedirect(std::ifstream &serverFile);
    ~LocationRedirect();

private:
    std::string url;
    std::string rootPath;
    bool directoryListing;
    std::string includeOne;
    std::string includeTwo;
    std::string fastCgiPass;
    std::string fastCgiParams;
    std::vector<std::string> defaultTryFiles;

    std::string extractUrl(std::ifstream &locationFile);
    std::string extractRoot(std::ifstream &locationFile);
    std::vector<std::string> extractTryFiles(std::ifstream &locationFile);
};