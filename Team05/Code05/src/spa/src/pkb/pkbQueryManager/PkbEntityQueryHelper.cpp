#include "PkbEntityQueryHelper.h"

vector<shared_ptr<PkbEntity>> PkbEntityQueryHelper::retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::PROCEDURE)->getAll();
}

vector<shared_ptr<PkbEntity>> PkbEntityQueryHelper::retrieveAllStatementEntities(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();
}

vector<shared_ptr<PkbEntity>> PkbEntityQueryHelper::retrieveAllVariables(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::VARIABLE)->getAll();
}

vector<shared_ptr<PkbEntity>> PkbEntityQueryHelper::retrieveAllConstants(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::CONSTANT)->getAll();
}

shared_ptr<PkbEntity> PkbEntityQueryHelper::retrieveEntityByTypeAndKey(string key, PkbEntityType type, shared_ptr<PkbRepository> repository) {
	return repository->getEntityTableByType(type)->get(key);
}