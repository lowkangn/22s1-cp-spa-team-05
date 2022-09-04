#include <string>
#include <list>
#include <fstream>

using namespace std;

#include <sp/dataclasses/tokens/Token.h>
#include <pkb/interfaces/PKBUpdateHandler.h>

class SourceProcessor {
public: 
	void tokenizeParseExtractAndUpdatePkb(istream& filestream, PKBUpdateHandler pkb);
};