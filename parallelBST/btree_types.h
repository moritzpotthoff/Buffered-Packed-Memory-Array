#pragma once

#include <boost/functional/hash.hpp>

#include <btree.h>

#include <tbb/cache_aligned_allocator.h>
#include <tbb/scalable_allocator.h>

#define TBB_ALLOCATOR_TRAITS_BROKEN 1

template <typename _Key, typename _Data, int _innerslots, int _leafslots = _innerslots + 1>
struct btree_map_traits
{
    /// If true, the tree will self verify it's invariants after each insert()
    /// or erase(). The header must have been compiled with BTREE_DEBUG defined.
    static const bool   selfverify = false;

    /// If true, the tree will print out debug information and a tree dump
    /// during insert() or erase() operation. The header must have been
    /// compiled with BTREE_DEBUG defined and key_type must be std::ostream
    /// printable.
    static const bool   debug = false;

    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    leafslots = _leafslots;

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    innerslots = _innerslots;

    /// As of stx-btree-0.9, the code does linear search in find_lower() and
    /// find_upper() instead of binary_search, unless the node size is larger
    /// than this threshold. See notes at
    /// http://panthema.net/2013/0504-STX-B+Tree-Binary-vs-Linear-Search
    static const size_t binsearch_threshold = 256;
};

template <typename _Key, typename _Data>
struct btree_32_256_map_traits
{
    /// If true, the tree will self verify it's invariants after each insert()
    /// or erase(). The header must have been compiled with BTREE_DEBUG defined.
    static const bool   selfverify = false;

    /// If true, the tree will print out debug information and a tree dump
    /// during insert() or erase() operation. The header must have been
    /// compiled with BTREE_DEBUG defined and key_type must be std::ostream
    /// printable.
    static const bool   debug = false;

    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    leafslots = 256;

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about 256 bytes.
    static const int    innerslots = 32;

    /// As of stx-btree-0.9, the code does linear search in find_lower() and
    /// find_upper() instead of binary_search, unless the node size is larger
    /// than this threshold. See notes at
    /// http://panthema.net/2013/0504-STX-B+Tree-Binary-vs-Linear-Search
    static const size_t binsearch_threshold = 256;
};


struct TBigStruct
{
    uint64_t BigStruct[64];

    TBigStruct(uint64_t num = 0)
    {
        BigStruct[0] = num;
    }

    operator uint64_t() const
    {
        return BigStruct[0];
    }

    TBigStruct& operator= (const TBigStruct& other)
    {
        std::copy(other.BigStruct, other.BigStruct + sizeof(other.BigStruct) / sizeof(uint64_t), BigStruct);
        return *this;
    }
};

namespace boost
{
template<>
struct hash<TBigStruct> : std::unary_function<TBigStruct, std::size_t>
{
    std::size_t operator()(const TBigStruct& val) const
    {
        return boost::hash_value((uint64_t) val);
    }
};
}

namespace std
{
template<>
struct numeric_limits<std::string>
{
    inline static const std::string min() noexcept {return "";}
    inline static const std::string max() noexcept {return "{";}
};

template<>
struct numeric_limits<TBigStruct>
{
    inline static const TBigStruct min() noexcept {return std::numeric_limits<uint64_t>::min();}
    inline static const TBigStruct max() noexcept {return std::numeric_limits<uint64_t>::max();}
};
}


using TKey = uint64_t;

struct data_type
{
};

template <bool Duplicate = true>
using TBtree_2_3 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 2>, Duplicate, tbb::scalable_allocator<TKey>, true>;


template <bool Duplicate = true>
using TBtree_2_4 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 3>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_4_8 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 7>, Duplicate, std::allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_8_16 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 15>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_16_32 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 31>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_32_64 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 63>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_64_128 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 127>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_128_256 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 255>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_256_512 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 511>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_large_leaf = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_map_traits<TKey, data_type, 3, 8192>, Duplicate, tbb::scalable_allocator<TKey>, true>;

template <bool Duplicate = true>
using TBtree_32_256 = stx::btree<TKey, data_type, TKey, std::less<TKey>,
        btree_32_256_map_traits<TKey, data_type>, Duplicate, tbb::cache_aligned_allocator<TKey>, true>;
