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
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// get stream 
	fstream stream;
	stream.open(filename);

	// parse

	stream.close();
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){


	// query

	// push into results list by reference


}
