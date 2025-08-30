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