#pragma once

#include <cstdint>
#include <random>

#include "assert.hpp"

namespace Definitions {
    using RandomEngine = std::mt19937_64;
    enum class OperationType : int { Insert = 0, Remove, Search, BatchInsert, BatchRemove };

    template<typename DataType>
    struct Batch {
        Batch(DataType *s, std::int64_t n) : start(s), size(n) {}

        DataType *start;
        std::int64_t size;
    };

    template<typename KeyType, typename ValueType = KeyType>
    struct Operation {
    public:
        OperationType type;
        union {
            KeyType value;//Single value for Insert, Remove or Search
            Batch<ValueType> batch;//The batch for batch operations.
        };
    public:
        Operation() : type(OperationType::Insert), value(0) {}

        Operation(OperationType op, KeyType v) : type(op), value(v) {
            AssertMsg(type != OperationType::BatchInsert && type != OperationType::BatchRemove, "Wrong operation type");
        }

        Operation(OperationType op, Batch<ValueType> b) : type(op), batch(b) {
            AssertMsg(type == OperationType::BatchInsert || type == OperationType::BatchRemove, "Wrong operation type " << (int)type);
        }
    };
}