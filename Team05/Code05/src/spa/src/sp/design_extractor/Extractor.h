#pragma once
#include <sp/dataclasses/AST.h>
#include <vector>
#include <memory>

template <typename T> class Extractor {
private:

public:
	virtual vector<T> extract(shared_ptr<ASTNode> rootnode) = 0;
};