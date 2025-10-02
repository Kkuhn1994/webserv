#pragma once

#include "ServerBlock.hpp"

#ifndef DEFAULT_CONFIG
# define DEFAULT_CONFIG "conf/nginxconf2"
#endif

class ConfService
{
public:
    ConfService();
    ~ConfService();
    

    void initialize(const std::string path);

    // std::vector<ServerBlock> getServerBlocks();

    std::vector<ServerBlock> serverBlock;
private:

    std::string path;

    void deleteCommentsOfConfig();
    void extractServerBlocks();
    void extractLocation(std::ifstream &file, int serverCount);
    void extractLocationBlocks();

    std::string extractPort(std::ifstream &serverFile);
    std::string extractServerName(std::ifstream &serverFile);
    std::string extractHostName(std::ifstream &serverFile);
};