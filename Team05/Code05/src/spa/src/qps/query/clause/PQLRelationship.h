#pragma once
#include <string>
#include "PQLEntity.h"

using namespace std;

class PQLRelationship {

private:

    PQLEntity firstEntity;
    PQLEntity secondEntity;

public:

    PQLRelationship(PQLEntity &firstEntity, PQLEntity &secondEntity): firstEntity(firstEntity), secondEntity(secondEntity) {
        this->firstEntity = firstEntity;
        this->secondEntity = secondEntity;
    }

    PQLEntity getFirstEntity() {
        return this->firstEntity;
    }

    PQLEntity getSecondEntity() {
        return this->secondEntity;
    }

    friend bool operator==(PQLRelationship first, PQLRelationship second);

};

