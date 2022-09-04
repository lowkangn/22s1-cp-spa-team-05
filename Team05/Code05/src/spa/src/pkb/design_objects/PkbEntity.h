#pragma once

#include <string>
#include <vector>
#include <sp/dataclasses/tokens/Token.h>

using namespace std;

enum class EntityType { STATEMENT, VARIABLE };

class PkbEntity {

public:
	// Use this method to generate variable entities.
	static PkbEntity generateVariable(string s) {
		return PkbEntity(s);
	}

	// Use this method to generate statement entities.
	static PkbEntity generateStatement(string s, int lineNumber) {
		return PkbEntity(s, lineNumber);
	}

	bool isStatement() {
		return this->type == EntityType::STATEMENT;
	}

	string toString() const {
		return this->string;
	}

	int getLineNumber() const {
		return this->lineNumber;
	}

	bool operator==(const PkbEntity &other) const {
		return (this->type == other.type
			&& this->string == other.string
			&& this->lineNumber == other.lineNumber);
	}

private:
	PkbEntity(string string) {
		this->type = EntityType::VARIABLE;
		this->string = string;
		// For variable entities, the line number is not used for comparison. Hence they are all set to 0 by default.
		this->lineNumber = 0;
	}

	PkbEntity(string string, int lineNumber) {
		this->type = EntityType::STATEMENT;
		this->string = string;
		this->lineNumber = lineNumber;
	}

	EntityType type;
	string string;
	int lineNumber;
};

class EntityHasher {

public:
	size_t operator()(const PkbEntity& entity) const {
		return (hash<string>()(entity.toString()))
			^ (hash<int>()(entity.getLineNumber()));
	}
};
