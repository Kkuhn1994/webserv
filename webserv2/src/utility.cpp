#include "utility.hpp"
#include <cctype>

std::vector<std::string> split(const std::string &str)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    bool in_token = false;

    for (size_t i = 0; i < str.length(); ++i) {
        if (std::isspace(str[i])) {
            if (in_token) {
                // Ende eines Tokens gefunden
                tokens.push_back(str.substr(start, end - start));
                in_token = false;
            }
            // Whitespace überspringen
            continue;
        }
        if (!in_token) {
            // Anfang eines neuen Tokens
            start = i;
            end = i;
            in_token = true;
        }
        end = i + 1;
    }

    // Letztes Token hinzufügen, falls vorhanden
    if (in_token) {
        tokens.push_back(str.substr(start, end - start));
    }

    return tokens;
}


std::vector<std::string> pathSplit(const std::string &str)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;

    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '/'){
                tokens.push_back(str.substr(start, end - start));
                start = i;
                end = i;
            }
        end = i + 1;
    }
    tokens.push_back(str.substr(start, end - start));
    return tokens;
}

std::string resolveWildcardErrorCode(const std::string& errorCode, const std::string& pathToPage) 
{
    std::string result = pathToPage;
    size_t codeIndex = errorCode.size() - 1;
    for (size_t i = result.size(); i > 0; --i) {
        if (result[i] == 'x' && codeIndex < errorCode.size()) {
            result[i] = errorCode[codeIndex];  // Ersetze 'x' durch die aktuelle Ziffer
            --codeIndex;
        }
    }
    return result;
}

std::string extractFile(std::ifstream &in)
{
	std::string text = "";
	std::string line;
	while (std::getline(in, line))
	{
		text += line;
		text += "\n";
	}
	return text;
}

std::string replace(std::string buffer, std::string s1, std::string s2)
{
	std::string replacedString;
	size_t occurence_of_s1;

	occurence_of_s1 = buffer.find(s1);
	while (occurence_of_s1 != std::string::npos)
	{
		replacedString.append(buffer.substr(0, occurence_of_s1));
		replacedString.append(s2);
		buffer = buffer.substr(occurence_of_s1 + s1.size(), buffer.size());
		occurence_of_s1 = buffer.find(s1);
	}
	replacedString.append(buffer);
	return replacedString;
}

#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Konnte Datei nicht öffnen: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string content;
    content.resize(size);
    if (!file.read(&content[0], size)) {
        throw std::runtime_error("Fehler beim Lesen der Datei: " + filename);
    }

    return content;
}

void deleteBlock(std::string fileName1, std::string fileName2, size_t locationSize)
{
	
	for(size_t index = 1;index <= locationSize; index ++)
	{
        std::string locationFileName;
        if(fileName2.find("limit_except"))
        {
            locationFileName = fileName2;
        }
        else
        {
            locationFileName = fileName2 + std::to_string(index) + ".txt";
        }
		
		std::ifstream fileIn(fileName1);
		std::ofstream fileOut;
		std::string inFile;
		std::string s1 = readFileToString(locationFileName);
		std::string s2 = "";
		std::string buffer;
		std::string		fileReplace;

		if (fileIn.good() == false)
		{
			std::cout << "Infile not availlable\n";
			return ;
		}
		inFile = fileName1;
		fileReplace = inFile + ".replace";
		fileOut.open(fileReplace);

		std::string text = "";
		while (std::getline(fileIn, buffer))
		{
			text += "\n" + buffer;
		}
		if (text.find(s1) == std::string::npos)
		{
			fileOut << text << std::endl;
		}
		else
			fileOut << replace(text, s1, s2) << std::endl;
		fileIn.close();
		fileOut.close();
		remove(fileName1.c_str());
		rename(fileReplace.c_str(), fileName1.c_str());
	}
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

bool starts_with_limit_except (const std::string &line)
{
    std::string trimmed = ltrim(line);
    return trimmed.rfind("limit_except ", 0) == 0; // prüft, ob "location" an Position 0 steht
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