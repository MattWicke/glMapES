#include "util.h"
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    std::vector<std::vector<std::string>> plists;
    std::string filename("testlist.plist");
    getPlist(filename, plists);

    for(size_t ii= 0; ii < plists.size(); ii++)
    {
        for(size_t jj= 0; jj < plists[ii].size(); jj++)
        {
            std::cout << plists[ii][jj] << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}
