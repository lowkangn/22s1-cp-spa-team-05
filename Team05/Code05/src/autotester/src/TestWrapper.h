#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "AbstractWrapper.h"
#include <common/FileIOManager.h>
#include <pkb/pkb_object/PKB.h>
#include <sp/SourceProcessor.h>

class TestWrapper : public AbstractWrapper {
private: 
	FileIOManager io;
	ProgramKnowledgeBase pkb;
	SourceProcessor sp;
public:
  // default constructor
  TestWrapper();
  
  // destructor
  ~TestWrapper();
  
  // method for parsing the SIMPLE source
  virtual void parse(std::string filename);
  
  // method for evaluating a query
  virtual void evaluate(std::string query, std::list<std::string>& results);
};

#endif
