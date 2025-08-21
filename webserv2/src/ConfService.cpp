#include "../include/ConfService.hpp"
#include <iostream>
#include <fstream>
#include <string>

ConfService::ConfService() : path(DEFAULT_CONFIG)
{
    std::cout << "ConfService created (default)\n";
}

ConfService::ConfService(const std::string Path) : path(Path)
{
    std::cout << "ConfService created (" << this->path << ")\n";
}

ConfService::~ConfService()
{
    std::cout << "ConfService destroyed\n";
}

void ConfService::initialize()
{
    deleteCommentsOfConfig();
    extractServerBlocks();
    extractLocationBlocks();
}

void ConfService::deleteCommentsOfConfig()
{
    std::ifstream file(this->path);
    std::ofstream tempFile("conf/tempConfig.txt", std::ios::app);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.length() == 0 || *line.begin() == '#')
            continue;
        for (char ch : line)
        {
            if (ch == '#')
            {
                break;
            }
            tempFile << ch;
        }
        tempFile << '\n';
    }
    tempFile.close();
    file.close();
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
    std::cout << result << std::endl;
    return result;
}

void ConfService::extractServerBlocks()
{
    std::ifstream file("conf/tempConfig.txt");
    std::string line;
    int serverCount = 0;

    while (std::getline(file, line))
    {
        if (starts_with_server(line))
        {
            serverCount++;

            std::string fileName = "conf/ServerConfig" + std::to_string(serverCount) + ".txt";
            std::ofstream tempFile(fileName, std::ios::app);
            tempFile << line;
            tempFile << '\n';
            tempFile << extractBlock(file, 1);
            tempFile.close();
        }
    }
    file.close();
}

void ConfService::extractLocation(std::ifstream &file, int serverCount)
{
    std::string line;
    int locationCount = 0;

    while (std::getline(file, line))
    {
        if (starts_with_location(line))
        {
            locationCount++;
            std::string fileName = "conf/locationConfig_" + std::to_string(serverCount) + "_" + std::to_string(locationCount) + ".txt";
            std::ofstream tempFile(fileName, std::ios::app);
            tempFile << line;
            tempFile << '\n';
            tempFile << extractBlock(file, 1);
            tempFile.close();
        }
    }
    file.close();
}

void ConfService::extractLocationBlocks()
{
    int counter = 1;
    while (1)
    {
        std::string fileName = "conf/ServerConfig" + std::to_string(counter) + ".txt";
        std::cout << fileName << std::endl;
        std::ifstream file(fileName);
        if (!file)
            break;
        else
            extractLocation(file, counter);
        counter++;
    }
}