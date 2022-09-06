#include <string>
#include <list>
#include <fstream>
#include <memory>

using namespace std;

#include <sp/dataclasses/tokens/Token.h>
#include <pkb/interfaces/PKBUpdateHandler.h>

class SourceProcessor {
public: 
	void tokenizeParseExtractAndUpdatePkb(istream& filestream, shared_ptr<PKBUpdateHandler> pkb);
};