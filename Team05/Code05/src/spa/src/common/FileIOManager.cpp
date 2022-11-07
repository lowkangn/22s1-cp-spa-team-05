#include <common/FileIOManager.h>

#include <fstream>
#include <string>

using namespace std;

void FileIOManager::readLinesFromFile(ifstream& stream, const string& filename) {
    stream.open(filename);
}
