#pragma once

#include "ServerBlock.hpp"

#ifndef DEFAULT_CONFIG
# define DEFAULT_CONFIG "conf/nginxconf"
#endif

class ConfService
{
public:
    ConfService();
    ConfService(const std::string path);
    ~ConfService();

    void initialize();

    std::vector<ServerBlock> serverBlock;
private:

    std::string path;

    void deleteCommentsOfConfig();
    void extractServerBlocks();
    void extractLocation(std::ifstream &file, int serverCount);
    void extractLocationBlocks();
};