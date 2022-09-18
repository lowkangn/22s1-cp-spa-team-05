#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

#include <string>
using namespace std;

const int DEAFULT_LINE_NUMBER = -1;
const int INVALID_LINE_NUMBER = -1;
const string INVALID_IDENTIFIER = "";

/*
	Enumeration of possible entity types.
*/
enum class EntityType { PROGRAM, PROCEDURE, STMTLIST, READ, PRINT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, EXPRESSION, BRACKET };

class Entity {
	private:
		EntityType type;
		int line;
		Token token;

		// Refer to https://stackoverflow.com/questions/68049611/no-default-constructor-exists-for-class-in-c for the following declaration usage
		Entity(EntityType type, int lineNumber, Token& token) : type(type), line(lineNumber), token(token) {
			this->type = type;
			this->line = lineNumber;
			this->token = token;
		}
	public:
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

		/*
			Gets the string in the token as an int.
		*/
		int getValue() {
			if (!this->isConstantEntity()) {
				throw logic_error("Entity is not a constant! No value to be gotten.");
			}
			return stoi(this->getString());
		}

		bool equals(Entity other) {
			return this->type == other.getType() && this->line == other.line && (this->token == other.token);
		}

		bool operator==(Entity other) {
			return this->type == other.getType() && this->line == other.line && (this->token == other.token);
		}

		bool isOperator() {
			return this->token.isRelationalOperator() || this->token.isOperatorToken()  || this->token.isNotOperator() || this->token.isConditionalOperator();
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
		
		bool isConstantEntity() {
			return this->type == EntityType::CONSTANT;
		}

		bool isBracketEntity() {
			return this->type == EntityType::BRACKET;
		}

		static Entity createProgramEntity() {
			Token token = Token::getPlaceHolderToken();
			return Entity(EntityType::PROGRAM, DEAFULT_LINE_NUMBER, token);
		}

		static Entity createProcedureEntity(Token token) {
			return Entity(EntityType::PROCEDURE, DEAFULT_LINE_NUMBER, token);
		}

		static Entity createStmtLstEntity() {
			Token token = Token::getPlaceHolderToken();
			return Entity(EntityType::STMTLIST, DEAFULT_LINE_NUMBER, token);
		}

		static Entity createReadEntity(int lineNumber) {
			Token token = Token::createReadToken();
			return Entity(EntityType::READ, lineNumber, token);
		}

		static Entity createPrintEntity(int lineNumber) {
			Token token = Token::createPrintToken();
			return Entity(EntityType::PRINT, lineNumber, token);
		}

		static Entity createAssignEntity(int lineNumber) {
			Token token = Token::createEqualsToken();
			return Entity(EntityType::ASSIGN, lineNumber, token);
		}

		static Entity createCallEntity(int lineNumber) {
			Token token = Token::createCallToken();
			return Entity(EntityType::CALL, lineNumber, token);
		}

		static Entity createWhileEntity(int lineNumber) {
			Token token = Token::createWhileToken();
			return Entity(EntityType::WHILE, lineNumber, token);
		}

		static Entity createIfEntity(int lineNumber) {
			Token token = Token::createIfToken();
			return Entity(EntityType::IF, lineNumber, token);
		}

		static Entity createVariableEntity(int lineNumber, Token token) {
			return Entity(EntityType::VARIABLE, lineNumber, token);
		}

		static Entity createConstantEntity(int lineNumber, Token token) {
			return Entity(EntityType::CONSTANT, lineNumber, token);
		}

		static Entity createExpressionEntity(int lineNumber, Token token) {
			return Entity(EntityType::EXPRESSION, lineNumber, token);
		}

		static Entity createBracketEntity() {
			Token token = Token::getPlaceHolderToken();
			return Entity(EntityType::BRACKET, DEAFULT_LINE_NUMBER, token);
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
