#include "LocationRedirect.hpp"

LocationRedirect::LocationRedirect(std::ifstream &locationFile) : rootPath("")
{
	std::string exceptionRequestRule;

	std::cout << "\n";
	statusCode = 200;
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
	locationFile.clear();
	locationFile.seekg(0, std::ios::beg);
	extractPossibleRequests(locationFile);
    std::cout << "URL: " << url << std::endl;
    std::cout << "Root Path: " << rootPath << std::endl;
    // std::cout << "Default Try Files:" << std::endl;
    // for (size_t i = 0; i < defaultTryFiles.size(); ++i) {
    // std::cout << "  [" << i << "]: " << defaultTryFiles[i] << std::endl;
    // }
	// std::cout << "FastCGIParams: "  << std::endl;
    // for (const auto& pair : fastCGIParam) {
    //     std::cout << pair.first << " : " << pair.second << std::endl;
    // }
    // std::cout << "FastCGIPass: " << fastCGIPass  << std::endl;
	// std::cout << "includeCGI: " << includeCGI  << std::endl;
	std::cout << "Request Statuscode: " << statusCode  << std::endl;
	std::cout << "Requestmessage: " << message  << std::endl;
	std::cout << "allowed methods: "  << std::endl;

	for (const auto& method : restrictedMethods) {
		std::cout << "\033[1;33m" << method << "\033[0m" << std::endl;
	}

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


void LocationRedirect::extractCGIStuff(std::ifstream &locationFile) 
{
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
			splitted = split(match[1]);
			fastCGIParam[splitted[0]] = splitted[1];
		}
	}
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
			files = split(match[1]);
			return files;
		}
	}
	return files;
}

std::string LocationRedirect::extractExceptRequest(std::ifstream &locationFile)
{
	std::regex exceptRegex(R"(limit_except\s+[^{]+\{\s*([^}]+?)\s*\})");
	std::string line;
	std::string fileContent;

	while (std::getline(locationFile, line))
	{
		fileContent += line + "\n";
	}
	std::smatch match;
	if (std::regex_search(fileContent, match, exceptRegex))
	{
		if (match.size() > 1)
		{ // Gruppe 1 enthält den Inhalt zwischen { }
			std::string bracketContent = match[1].str();
			bracketContent.erase(bracketContent.find_last_not_of(" \t\n\r\f\v") + 1);
			bracketContent.erase(0, bracketContent.find_first_not_of(" \t\n\r\f\v"));
			return bracketContent;
		}
	}
	return "";
}

void LocationRedirect::extractPossibleRequests(std::ifstream &locationFile)
{
	std::vector<std::string> request;
	std::regex locationRegex(R"(limit_except\s+([^;]+))");
	std::regex returnRegex(R"(return\s+(\d+)\s+\"([^\"]+)\";)");
	std::string line;
	std::string store;

	while (std::getline(locationFile, line))
	{
		std::smatch match;
		if (std::regex_search(line, match, locationRegex) && match.size() > 1)
		{
			request = split(match[1]);
			request.pop_back();
			restrictedMethods = request;
			locationFile.clear();
			locationFile.seekg(0, std::ios::beg);
			store = extractExceptRequest(locationFile);
			if (store.find("deny all;") != std::string::npos)
			{
				statusCode = 403;
				message = "Access denied by server configuration";
				return ;
			}
			else
			{
				if (std::regex_search(line, match, returnRegex))
				{
					if (match.size() >= 3)
					{
						statusCode = std::stoi(match[1].str());
						message = match[2].str();
					}
				}
				else
				{
					statusCode = 403;
					message = "Access denied";
				}
				return ;
			}
		}
	}
}

std::string LocationRedirect::getUrl()
{
	return url;
}

std::string LocationRedirect::getRoot()
{
	return rootPath;
}

std::vector<std::string>    LocationRedirect::getRestrictedMethods()
{
	return restrictedMethods;
}

int		LocationRedirect::getStatusCode()
{
	return statusCode;
}

std::string LocationRedirect::getMessage()
{
	return message;
}