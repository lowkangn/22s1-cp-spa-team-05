#pragma once

#include <istream>
#include <string>

using namespace std;

class FileIOManager {
public:
    /*
        Reads in a file stream by reference. Note that any errors in reading 
        will be thrown and propagated to the caller.
    */
    void readLinesFromFile(ifstream& stream, const string& filename);
};
