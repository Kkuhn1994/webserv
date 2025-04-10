#include "ServerBlock.hpp"

class ConfService
{
public:
    ConfService();
    ~ConfService();

    void initialize();

private:
    std::vector<ServerBlock> serverBlock;

    void deleteCommentsOfConfig();
    void extractServerBlocks();
};