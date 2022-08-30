#include <iostream>
#include <fstream>
#include <FileIOManager.h>
#include <vector>
#include <string>

using namespace std;

vector<string> FileIOManager::readLinesFromFile(string filename) {

    // initialize
    string line;
    vector<string> lines;

    // open file
    // we intentionally don't wrap this in a try-catch, so that error states can be handled by 
    // caller
    ifstream file(filename);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }

    return lines;
}