#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/design_objects/Entity.h>

using namespace std;


const int DEAFULT_LINE_NUMBER = -1;

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

		bool isBracketEntity() {
			return this->type == EntityType::BRACKET;
		}

		static Entity createProgramEntity() {
			return Entity(EntityType::PROGRAM, DEAFULT_LINE_NUMBER, Token::getPlaceHolderToken());
		}

		static Entity createProcedureEntity(Token token) {
			return Entity(EntityType::PROCEDURE, DEAFULT_LINE_NUMBER, Token::getPlaceHolderToken());
		}

		static Entity createStmtLstEntity() {
			return Entity(EntityType::STMTLIST, DEAFULT_LINE_NUMBER, Token::getPlaceHolderToken());
		}

		static Entity createReadEntity(int lineNumber) {
			return Entity(EntityType::READ, lineNumber, Token(READ_KEYWORD, TokenType::NAME_OR_KEYWORD));
		}

		static Entity createPrintEntity(int lineNumber) {
			return Entity(EntityType::PRINT, lineNumber, Token(PRINT_KEYWORD, TokenType::NAME_OR_KEYWORD));
		}

		static Entity createAssignEntity(int lineNumber) {
			return Entity(EntityType::ASSIGN, lineNumber, Token(EQUAL_OPERATOR, TokenType::OPERATOR));
		}

		static Entity createCallEntity(int lineNumber) {
			return Entity(EntityType::CALL, lineNumber, Token(CALL_KEYWORD, TokenType::NAME_OR_KEYWORD));
		}

		static Entity createWhileEntity(int lineNumber) {
			return Entity(EntityType::WHILE, lineNumber, Token(WHILE_KEYWORD, TokenType::NAME_OR_KEYWORD));
		}

		static Entity createIfEntity(int lineNumber) {
			return Entity(EntityType::WHILE, lineNumber, Token(IF_KEYWORD, TokenType::NAME_OR_KEYWORD));
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
			return Entity(EntityType::BRACKET, DEAFULT_LINE_NUMBER, Token::getPlaceHolderToken());
		}
};
