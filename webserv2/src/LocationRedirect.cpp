#include "LocationRedirect.hpp"

LocationRedirect::LocationRedirect(std::ifstream &locationFile)
{
	std::string exceptionRequestRule;

	url = extractUrl(locationFile);
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	rootPath = extractRoot(locationFile);
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	defaultTryFiles = extractTryFiles(locationFile);
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	extractCGIStuff(locationFile);
    std::cout << "URL: " << url << std::endl;
    std::cout << "Root Path: " << rootPath << std::endl;
    std::cout << "Default Try Files:" << std::endl;
    for (size_t i = 0; i < defaultTryFiles.size(); ++i) {
    std::cout << "  [" << i << "]: " << defaultTryFiles[i] << std::endl;
    }
	std::cout << "FastCGIParam: " << fastCGIParam1 << std::endl;
    std::cout << "FastCGIParam2: " <<  fastCGIParam2 << std::endl;
    std::cout << "FastCGIPass: " << fastCGIPass  << std::endl;
	std::cout << "includeCGI: " << includeCGI  << std::endl;
    std::cout << std::endl;
}

LocationRedirect::~LocationRedirect()
{
	// if(possibleRequests != NULL)
	// {
	// 	delete possibleRequests;
	// }
	// if(fastCGIStuff != NULL)
	// {
	// 	delete fastCGIStuff;
	// }
	// std::cout << BLUE << "redirect destroyed\n"
	// 		  << RESET;
}


void LocationRedirect::extractCGIStuff(std::ifstream &locationFile) {
	extractCGIPass(locationFile);
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	extractCGIParams(locationFile);
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	extractInclude(locationFile);
}


void LocationRedirect::setCGIPass(std::string _fastCGIPass)
{
    fastCGIPass = _fastCGIPass;
    // std::cout << fastCGIPass << std::endl;
}

std::string LocationRedirect::getCGIPass()
{
    return fastCGIPass;
}

void LocationRedirect::setCGIParam1(std::string _fastCGIParam)
{
    fastCGIParam1 = _fastCGIParam;
    // std::cout << fastCGIParam1 << std::endl;
}

std::string LocationRedirect::getCGIParam1()
{
    return fastCGIParam1;
}

void LocationRedirect::setCGIParam2(std::string _fastCGIParam)
{
    fastCGIParam2 = _fastCGIParam;
    // std::cout << fastCGIParam2 << std::endl;
}

std::string LocationRedirect::getCGIParam2()
{
    return fastCGIParam2;
}

void LocationRedirect::setInclude(std::string _Include)
{
    includeCGI = _Include;
    // std::cout << include << std::endl;
}

std::string LocationRedirect::getInclude()
{
    return includeCGI;
}



void LocationRedirect::extractCGIPass(std::ifstream &locationFile) {
    std::regex cgiPassRegex(R"(fastcgi_pass\s+(?:~|=\s*)?([^\s{;]+))");

    std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, cgiPassRegex))
		{
			setCGIPass(match[1]);
			return;
		}
	}
	setCGIPass("");
}

void LocationRedirect::extractCGIParams(std::ifstream &locationFile) {
    std::regex cgiPassRegex(R"(fastcgi_param\s+([^;]+))");
    std::vector<std::string> splitted;
    std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, cgiPassRegex))
		{
			splitted = split(match[1], " ");
            setCGIParam1(splitted[0]);
            setCGIParam2(splitted[1]);
            return;
		}
	}
	setCGIParam1("");
    setCGIParam2("");
}

void LocationRedirect::extractInclude(std::ifstream &locationFile) {
    std::regex cgiPassRegex(R"(include\s+(?:~|=\s*)?([^\s{;]+))");

    std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, cgiPassRegex))
		{
			setInclude(match[1]);
			return;
		}
	}
	setInclude("");
}


std::string LocationRedirect::extractUrl(std::ifstream &locationFile)
{
	std::regex pattern(R"(location\s+(.*?)\s*\{)");
	std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, pattern))
		{
			return match[1].str();
		}
	}
	return "";
}

std::string LocationRedirect::extractRoot(std::ifstream &locationFile)
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


std::vector<std::string> LocationRedirect::extractTryFiles(std::ifstream &locationFile)
{
	std::vector<std::string> files;
	std::regex locationRegex(R"(try_files\s+([^;]+))");
	std::string line;
	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, locationRegex) && match.size() > 1)
		{
			files = split(match[1], " ");
			return files;
		}
	}
	return files;
}