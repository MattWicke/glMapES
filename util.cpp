#include "util.h"
#include <stdio.h>

using namespace std;

/*
 * Read in from a file name and create an char array from
 * the file. check for errors
 */
bool file_reader(const char* fileName, string& contents, int &length)
{
   ifstream fileStream(fileName);

   if(fileStream.is_open())
   {
      string n_line;
      while(getline(fileStream, n_line))
      {
         contents += n_line += "\n";
      }

      //contents += EOF;
      fileStream.close();
      length = contents.length();
      return true;
   }


   cerr << "file_reader: Failed to open the file" << endl;

   return false;
}



//************************* playlist file io stuff ***********************************
bool getCoords(vector<vector<float> >& data,  string _fileName)
{
   bool success = false;
   int surfaceCount = 0; // the totall number of surfaces to create
   int index = 0;
   string inString;
   int length;

   file_reader(_fileName.c_str(), inString, length);

   do
   {
      vector<float> tempCoords;
      //find the word surface
      int retVal = inString.find("surface", index); 
      if( retVal > -1)
      {
         index = retVal + 8;
         surfaceCount++;
      }

      for(int i = 0; i < 8; i++)
      {
         float readNumber = atof(inString.substr(index, inString.find(',',index)).c_str());
         index = inString.find(',', index);
         index++;
         tempCoords.push_back(readNumber);
      }
      data.push_back(tempCoords);
   }
   while(inString.find("surface", index) != -1);
   
   return success;
}

bool writeCoordsFile(string _fileName, const vector<vector<float> > _data)
{
   ofstream fileStream(_fileName.c_str());

   for(int i = 0; i < _data.size(); i++)
   {
      fileStream << "surface\n";
      for(int j = 0; j < _data[i].size(); j++)
      {
         fileStream <<  _data[i][j] << ',';
         if(j % 2)
            fileStream << endl;
      }
      fileStream << endl;
   }
   fileStream.close();
}

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
            
            do
            {
                int repeat = 0;
                std::string line;
                std::stringstream ss(all_lines[ii]);
                //std::cout << ss.str() << endl;
                ss >> line;
                ss >> repeat;
                //std::cout << line << " count " << repeat << endl;
                for(int jj = 0; jj < repeat; jj++)
                    small_list.push_back(line);
                ii++;
            }while(all_lines[ii].compare("]") != 0);
            plists.push_back(small_list);
        }
        ii++;
    }

   return success;
}

