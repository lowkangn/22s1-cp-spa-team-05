#include <qps/query_parser/Declaration.h>

unordered_map<string, DesignEntity> entityMap({
	{"procedure", DesignEntity::PROCEDURE},
	{"stmt", DesignEntity::STMT},
	{"read", DesignEntity::READ},
	{"print", DesignEntity::PRINT},
	{"assign", DesignEntity::ASSIGN},
	{"call", DesignEntity::CALL},
	{"while", DesignEntity::WHILE},
	{"if", DesignEntity::IF},
	{"variable", DesignEntity::VARIABLE},
	{"constant", DesignEntity::CONSTANT},
});

Declaration::Declaration(string designEntity, string synonym) {
	try {
		this->designEntity = entityMap.at(designEntity);
	} catch (const out_of_range& oor) {
		throw PQLError(designEntity + " is not supported as a Design Entity");
	}
	this->synonym = synonym;
}

bool operator== (Declaration firstDeclaration, Declaration otherDeclaration) {
	return firstDeclaration.getDesignEntity() == otherDeclaration.getDesignEntity()
		&& firstDeclaration.getSynonym() == otherDeclaration.getSynonym();
}