#include <fstream>
#include <string>
#include <FileIOManager.h>


using namespace std;

void FileIOManager::readLinesFromFile(ifstream& stream, const string& filename) {

    try {
        stream.open(filename);
    }
    catch (exception e) {
        exit(0);
    }
};
