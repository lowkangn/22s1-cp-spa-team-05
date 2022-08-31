#include <fstream>
#include <string>
#include <FileIOManager.h>


using namespace std;

void FileIOManager::readLinesFromFile(ifstream& stream, const string& filename) {

	stream.open(filename);

};
