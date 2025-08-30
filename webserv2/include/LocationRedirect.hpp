#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include "utility.hpp"


class LocationRedirect
{
public:
    LocationRedirect(std::ifstream &serverFile);
    ~LocationRedirect();

    std::string getUrl();
    std::string getRoot();

private:
    std::string url;
    std::string rootPath;
    bool directoryListing;

    std::vector<std::string> defaultTryFiles;

    std::string fastCGIPass;
    std::map<std::string, std::string> fastCGIParam;
    std::string includeCGI;

    std::vector<std::string> allowedMethods;
    int statusCode;
    std::string message;

    std::string extractUrl(std::ifstream &locationFile);
    std::string extractRoot(std::ifstream &locationFile);
    void extractCGIStuff(std::ifstream &locationFile);
    std::vector<std::string> extractTryFiles(std::ifstream &locationFile);
    std::string extractExceptRequest(std::ifstream &locationFile);
    void extractPossibleRequests(std::ifstream &locationFile);
    void extractCGIPass(std::ifstream &locationFile);
    void extractCGIParams(std::ifstream &locationFile);
    void extractInclude(std::ifstream &locationFile);
    
    void setCGIPass(std::string _fastCGIPass);
    std::string getCGIPass();
    void setCGIParam1(std::string _fastCGIParam);
    std::string getCGIParam1();
    void setCGIParam2(std::string _fastCGIParam);
    std::string getCGIParam2();
    void setInclude(std::string _Include);
    std::string getInclude();


};