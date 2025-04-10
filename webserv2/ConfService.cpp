#include "ConfService.hpp"
#include <iostream>
#include <fstream>
#include <string>

ConfService::ConfService()
{
    std::cout << "ConfService created\n";
}

ConfService::~ConfService()
{
    std::cout << "ConfService destroyed\n";
}

void ConfService::initialize()
{
    deleteCommentsOfConfig();
}

void ConfService::deleteCommentsOfConfig()
{
    std::ifstream file("nginxconf");
    std::ofstream tempFile("tempConfig.txt", std::ios::app);
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

void ConfService::extractServerBlocks()
{
    std::ifstream file("tempConfig.txt");
}