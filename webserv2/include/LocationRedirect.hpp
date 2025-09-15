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
    LocationRedirect();
    ~LocationRedirect();

    void                        initialize(int index1, int index2,std::ifstream &serverFile);
    std::string                 getUrl();
    std::string                 getRoot();
    std::vector<std::string>    getRestrictedMethods();
	std::string                 getMessage();
    std::string                 isRedirected();
	int							getStatusCode();
    std::vector<std::string>    getIndexFiles();
    std::map<std::string, std::string> getCGIParams();  // CGI ADDITION: Public getter for CGI environment variables from config


private:
    std::string                 		url;
    std::string                 		rootPath;
    bool                        		directoryListing;

    std::vector<std::string>    		defaultTryFiles;
    std::vector<std::string>            indexFiles;

    std::string 						fastCGIPass;
    std::map<std::string, std::string> 	fastCGIParam;
    std::string 						includeCGI;

    std::vector<std::string> 			restrictedMethods;
    int 								statusCode;
    std::string 						message;
    std::string                         redirection;
    
    void extractRedirections(std::ifstream &locationFile);
    std::string extractUrl(std::ifstream &locationFile);
    std::string extractRoot(std::ifstream &locationFile);
    void extractCGIStuff(std::ifstream &locationFile);
    std::vector<std::string> extractTryFiles(std::ifstream &locationFile);
    std::string extractExceptRequest(std::ifstream &locationFile);
    void extractPossibleRequests(std::ifstream &locationFile, int index1, int index2);
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

    void initIndexFiles(std::ifstream &serverFile);

};