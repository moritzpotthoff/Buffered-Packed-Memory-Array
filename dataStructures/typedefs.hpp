#pragma once

#include <cstdint>

/**
 * If true, uses one array of structs with block pointer and head value.
 * Value false is currently not supported. It should use three arrays,
 * one with block pointers, one with head values, one with sizes.
 * Default is true.
 */
#define USE_INLINE_MEMORY_LAYOUT true

namespace DataStructures {
    //Only used for old implementations
    static constexpr uint64_t NULL_VALUE = 0;
    //Shortcuts for types used in the implementations
    using IndexType = std::int64_t;
    using SizeType = std::int64_t;
    using UnsignedIndexType = std::uint64_t;
    using UnsignedSizeType = std::uint64_t;
    using BlockSizeType = std::uint16_t;
    using BlockIndexType = std::uint16_t;
    using SignedBlockIndexType = std::int16_t;
    using UnsignedThreadIndex = std::uint16_t;
    using SignedThreadIndex = std::int16_t;
}