#pragma once
#include <sp/dataclasses/cfg/CFGNode.h>

#include <memory>
#include <vector>

template <typename T>
class CFGExtractor {
private:
public:
    virtual vector<T> extract(shared_ptr<CFGNode> rootnode) = 0;
};
