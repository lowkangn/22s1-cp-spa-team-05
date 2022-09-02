#pragma once
#include <sp/dataclasses/AST.h>
#include <vector>

template <typename T> class Extractor {
private:

public:
	virtual vector<T> extract(ASTNode rootnode) = 0;
};