#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;

/*
	Enumeration of possible entity types.
*/
enum class EntityType { PROGRAM, PROCEDURE, STMTLIST, READ, PRINT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, EXPRESSION };

class Entity {
	private:
		EntityType type;
		int line;
		Token token;
	public:
		// Refer to https://stackoverflow.com/questions/68049611/no-default-constructor-exists-for-class-in-c for the following declaration usage
		Entity(EntityType type, int lineNumber, Token& token) : type(type), line(lineNumber), token(token) {
			this->type = type;
			this->line = lineNumber;
			this->token = token;
		}

		EntityType getType() {
			return this->type;
		}

		int getLine() {
			return this->line;
		}

		string toString() {
			return this->token.getString();
		}

		bool equals(Entity& other) {
			return this->type == other.getType() && this->line == other.line && (this->token == other.token);
		}

		bool isProgramEntity() {
			return this->type == EntityType::PROGRAM;
		}

		bool isStmtLstEntity() {
			return this->type == EntityType::STMTLIST;
		}		
		
		bool isConstantEntity() {
			return this->type == EntityType::CONSTANT;
		}
};
