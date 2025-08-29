#include <vector>
#include <string>
#include <fstream>

std::vector<std::string> split(const std::string &str);
std::vector<std::string> pathSplit(const std::string &str);
std::string resolveWildcardErrorCode(const std::string& errorCode, const std::string& pathToPage);
std::string extractFile(std::ifstream &in);