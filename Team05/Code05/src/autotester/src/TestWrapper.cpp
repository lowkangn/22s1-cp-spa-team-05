#include "TestWrapper.h"
#include <istream>
#include <fstream>
#include <memory>

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
	this->shouldAnswerQuery = true;
	this->pkbPointer = shared_ptr<PKB>(new PKB(true));
	this->qps = QPS();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// get stream 
	fstream stream;
	stream.open(filename);
	
	try{
		//parse
		SourceProcessor sp = SourceProcessor();
		sp.initialize(stream);
		sp.extractAllAndAddToPkb(pkbPointer);
	}
	catch (SPException e) {
		stream.close();
		exit(1);
	}
	catch (SimpleSyntaxParserException e) {
		stream.close();
		exit(1);
	}
	catch (ASTException e) {
		stream.close();
		exit(1);
	}
	stream.close();

}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
	// query
	this->qps.evaluate(query, shared_ptr<PKBQueryHandler>(pkbPointer));

	// push into results list by reference
	this->qps.projectResults(results);

    pkbPointer->resetCache();
}
