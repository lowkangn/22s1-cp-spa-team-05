#include <iostream>
#include <fstream>
#include <istream>
#include <FileIOManager.h>
#include <string>

using namespace std;

istream FileIOManager::readLinesFromFile(string &filename) {

    try {
        fstream file;
        file.open(filename);
        istream& fileStream = file;
        return fileStream;

    }
    catch (exception e) {
        exit(0);
    }
}
}