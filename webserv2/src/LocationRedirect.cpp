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
    std::cout << "URL: " << url << std::endl;
    std::cout << "Root Path: " << rootPath << std::endl;
    std::cout << "Default Try Files:" << std::endl;
    for (size_t i = 0; i < defaultTryFiles.size(); ++i) {
    std::cout << "  [" << i << "]: " << defaultTryFiles[i] << std::endl;
    }
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

std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));

	return tokens;
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