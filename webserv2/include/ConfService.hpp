#pragma once

#include "ServerBlock.hpp"

class ConfService
{
public:
    ConfService();
    ~ConfService();

    void initialize();

    std::vector<ServerBlock> getServerBlocks();

private:
    std::vector<ServerBlock> serverBlock;

    void deleteCommentsOfConfig();
    void extractServerBlocks();
    void extractLocation(std::ifstream &file, int serverCount);
    void extractLocationBlocks();

    std::string extractPort(std::ifstream &serverFile);
    std::string extractServerName(std::ifstream &serverFile);
    std::string extractHostName(std::ifstream &serverFile);
};