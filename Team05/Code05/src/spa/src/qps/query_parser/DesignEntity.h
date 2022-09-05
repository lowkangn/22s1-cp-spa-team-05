#pragma once

#include <string>

#include <qps/query_tokenizer/PQLToken.h>

using namespace std;

enum class DesignEntity {
	PROCEDURE,
	STMT,
	READ,
	PRINT,
	ASSIGN,
	CALL,
	WHILE,
	IF,
	VARIABLE,
	CONSTANT
};

DesignEntity getDesignEntity(string s);
bool isStatementDesignEntity(DesignEntity designEntity);