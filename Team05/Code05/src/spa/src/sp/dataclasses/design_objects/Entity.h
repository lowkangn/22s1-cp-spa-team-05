#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

#include <string>
using namespace std;

const int INVALID_LINE_NUMBER = -1;
const string INVALID_IDENTIFIER = "";

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

		/*
			Converts object to a string representation.
		*/
		string toString() {
			return this->token.getString();
		}

		/*
			Gets the tokens as string.
		*/
		string getString() {
			return this->token.getString();
		}

		bool equals(Entity& other) {
			return this->type == other.getType() && this->line == other.line && (this->token == other.token);
		}

		bool isProgramEntity() {
			return this->type == EntityType::PROGRAM;
		}

		bool isProcedureEntity() {
			return this->type == EntityType::PROCEDURE;
		}

		bool isStmtLstEntity() {
			return this->type == EntityType::STMTLIST;
		}

		
		bool isReadEntity() {
			return this->type == EntityType::READ;
		}

		bool isPrintEntity() {
			return this->type == EntityType::PRINT;
		}
		bool isAssignEntity() {
			return this->type == EntityType::ASSIGN;
		}
		bool isCallEntity() {
			return this->type == EntityType::CALL;
		}
		bool isIf() {
			return this->type == EntityType::IF;
		}
		bool isVariableEntity() {
			return this->type == EntityType::VARIABLE;
		}

		bool isWhile() {
			return this->type == EntityType::WHILE;
		}

		bool isStmtEntity() {
			return (
				this->isAssignEntity()
				|| this->isCallEntity()
				|| this->isIf()
				|| this->isPrintEntity()
				|| this->isReadEntity()
				|| this->isWhile()
				);
		}
		
};
