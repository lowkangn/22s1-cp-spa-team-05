#include "PkbEntityQueryHelper.h"

vector<PkbEntity> PkbEntityQueryHelper::retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::PROCEDURE)->getAll();
}

vector<PkbEntity> PkbEntityQueryHelper::retrieveAllStatementEntities(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();
}

vector<PkbEntity> PkbEntityQueryHelper::retrieveAllVariables(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::VARIABLE)->getAll();
}

vector<PkbEntity> PkbEntityQueryHelper::retrieveAllConstants(shared_ptr<PkbRepository> repository)
{
	return repository->getEntityTableByType(PkbEntityType::CONSTANT)->getAll();
}

vector<PkbEntity> PkbEntityQueryHelper::retrieveEntityByTypeAndKey(string key, PkbEntityType type, shared_ptr<PkbRepository> repository) {
	return repository->getEntityTableByType(type).get(key);
}