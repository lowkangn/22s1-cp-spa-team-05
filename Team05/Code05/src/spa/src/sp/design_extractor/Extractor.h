#pragma once
#include <sp/dataclasses/AST.h>

template <typename T> class Extractor {
private:

public:
	virtual T extract(AST rootnode) = 0;
};