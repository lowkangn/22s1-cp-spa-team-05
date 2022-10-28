#pragma once
#include <sp/dataclasses/cfg/CFGNode.h>
#include <vector>
#include <memory>

template <typename T> class CFGExtractor {
private:

public:
	virtual vector<T> extract(shared_ptr<CFGNode> rootnode) = 0;
};