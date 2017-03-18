#include "readFunc.h"
bool getPlist(std::string filename, std::vector<std::vector<std::string>>& plists) 
{
    ifstream fileStream(filename);
    bool success = false;
    std::vector<std::string> all_lines;

    if(fileStream.is_open())
    {
        string n_line;
        while(getline(fileStream, n_line))
        {
            all_lines.push_back(n_line);
            //cout << n_line << endl;
        }
 
        fileStream.close();
    }

    int ii = 0;
    while(ii < all_lines.size()) 
    {
        std::vector<std::string> small_list;
        if(all_lines[ii].compare("[") == 0)
        {
            ii++;

            while(all_lines[ii].compare("]") != 0)
            {
                //std::cout << all_lines[ii] << endl;
                if(ii < all_lines.size())
                    small_list.push_back(all_lines[ii]);
                ii++;
            }
            plists.push_back(small_list);
        }
        ii++;
    }

   return success;
}
