#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define C_GRAY "\e[5;2;38;240;23;23m"
#define C_RED "\e[38;2;123;23;23m"
#define C_GREE "\e[38;2;23;123;23m"
#define C_NONE "\e[0m"

std::vector<std::string> split(const std::string &str);
std::vector<std::string> pathSplit(const std::string &str);
std::vector<std::string> parameterSplit(const std::string &str);
std::string resolveWildcardErrorCode(const std::string& errorCode, const std::string& pathToPage);
std::string extractFile(std::ifstream &in);
void deleteBlock(std::string fileName1, std::string fileName2, size_t locationSize);
std::string extractBlock(std::ifstream &in, int brace_count);
bool starts_with_location(const std::string &line);
bool starts_with_server(const std::string &line);
bool starts_with_limit_except (const std::string &line);
