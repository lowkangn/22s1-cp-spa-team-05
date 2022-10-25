#pragma once

#include <pkb/pkbRepository/design_objects/patterns/PkbPattern.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbIfPattern.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbAssignPattern.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbWhilePattern.h>
#include <pkb/pkbRepository/PkbRepository.h>

#include <qps/query/clause/ClauseArgument.h>

#include <vector>
#include <memory>

using namespace std;

class PkbPatternQueryHelper {
public: 

	vector<shared_ptr<PkbPattern>> retrieveAssignPatterns(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	vector<shared_ptr<PkbPattern>> retrieveIfPatterns(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);

	vector<shared_ptr<PkbPattern>> retrieveWhilePatterns(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository);
};