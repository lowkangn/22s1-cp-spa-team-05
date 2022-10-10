#pragma once

#include <memory>
#include <vector>
#include <string>

#include <pkb/design_objects/entities/PkbEntity.h>
using namespace std;

class PkbGraphNode {

protected:
	shared_ptr<PkbEntity> entity; // encapsulated entity
	vector<shared_ptr<PkbGraphNode>> children; // children
	PkbGraphNode(shared_ptr<PkbEntity> entity) {
		this->entity = entity;
	}	

public:
	/*
		Adds a child to the node. Note that there is no notion of order in 
		the children.
	*/
	void addNeighbour(shared_ptr<PkbGraphNode> node) {
		this->children.push_back(entity);
	}

	vector<shared_ptr<PkbGraphNode>> getNeighbours() {
		return this->children;
	}

	/*
		Returns a unique key identifying the node.
	*/
	virtual string getKey() = 0;

	bool equals(shared_ptr<PkbGraphNode> other) {
		return this->entity->equals(other->entity);
	}



};