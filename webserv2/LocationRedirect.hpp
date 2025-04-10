#include <iostream>
#include <map>
#include <vector>
#include <string>

class LocationRedirect
{
private:
    std::string url;
    std::string redirectToPath;
    bool directoryListing;
    std::string includeOne;
    std::string includeTwo;
    std::string fastCgiPass;
    std::string fastCgiParams;
};