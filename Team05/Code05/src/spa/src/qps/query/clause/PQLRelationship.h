#pragma once
#include <qps/query/clause/PQLEntity.h>

#include <string>
#include <utility>

using namespace std;

class PQLRelationship {
private:
    PQLEntity firstEntity;
    PQLEntity secondEntity;

public:
    explicit PQLRelationship(PQLEntity& firstEntity, PQLEntity& secondEntity)
        : firstEntity(firstEntity), secondEntity(secondEntity) {
    }

    PQLRelationship(const PQLRelationship& other) = default;

    PQLRelationship(PQLRelationship&& other) noexcept : firstEntity(move(other.firstEntity)),
                                                        secondEntity(move(other.secondEntity)) {
    }

    PQLRelationship& operator=(const PQLRelationship&) = default;

    PQLRelationship& operator=(PQLRelationship&&) = default;

    PQLEntity getFirstEntity() const {
        return this->firstEntity;
    }

    PQLEntity getSecondEntity() const {
        return this->secondEntity;
    }

    friend bool operator==(PQLRelationship first, PQLRelationship second);

    friend bool operator<(PQLRelationship first, PQLRelationship second);
};
