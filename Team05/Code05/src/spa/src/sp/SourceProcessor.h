#include <string>
#include <list>
#include <fstream>
#include <memory>
#include <sp/dataclasses/tokens/Token.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
using namespace std;

class SourceProcessor {
public: 
	void tokenizeParseExtractAndUpdatePkb(istream& filestream, shared_ptr<PKBUpdateHandler> pkb);
};