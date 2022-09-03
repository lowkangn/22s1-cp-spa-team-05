#pragma once

#include <string>
#include <vector>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

enum class EntityType { STATEMENT, VARIABLE };

class PkbEntity {

public:
	//TODO: Implement factory methods instead of overloaded constructors
	
	// Use to create variable entities.
	PkbEntity(string string) {
		this->type = EntityType::VARIABLE;
		this->string = string;
		// For variable entities, the line number is not used for comparison. Hence they are all set to 0 by default.
		this->lineNumber = 0;
	}

	// Use to create statement entities.
	PkbEntity(string string, int lineNumber) {
		this->type = EntityType::STATEMENT;
		this->string = string;
		this->lineNumber = lineNumber;
	}

	bool isStatement() {
		return this->type == EntityType::STATEMENT;
	}

	string toString() {
		return this->string;
	}

	int getLineNumber() {
		return this->lineNumber;
	}

	bool operator==(const PkbEntity &other) const {
		return (this->type == other.type
			&& this->string == other.string
			&& this->lineNumber == other.lineNumber);
	}

private:
	EntityType type;
	string string;
	int lineNumber;
};

class EntityHasher {

public:
	size_t operator()(PkbEntity entity) const {
		return (hash<string>()(entity.toString()))
			^ (hash<int>()(entity.getLineNumber()));
	}
};
