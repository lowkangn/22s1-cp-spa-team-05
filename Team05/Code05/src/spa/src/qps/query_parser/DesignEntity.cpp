#include <qps/query_parser/DesignEntity.h>

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
	}
);

DesignEntity getDesignEntity(string designEntity) {
	try {
		return entityMap.at(designEntity);
	}
	catch (const out_of_range& oor) {
		throw PQLError(designEntity + " is not supported as a Design Entity");
	}
};

bool isStatementDesignEntity(DesignEntity designEntity) {
	return  designEntity == DesignEntity::STMT || designEntity == DesignEntity::READ
		|| designEntity == DesignEntity::PRINT || designEntity == DesignEntity::ASSIGN
		|| designEntity == DesignEntity::CALL || designEntity == DesignEntity::WHILE
		|| designEntity == DesignEntity::IF;
};