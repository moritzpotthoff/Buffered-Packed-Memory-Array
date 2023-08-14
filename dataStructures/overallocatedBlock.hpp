#pragma once

#include <array>

#include "assert.hpp"
#include "typedefs.hpp"
#include "iterators.hpp"

namespace DataStructures {
template<typename KEY, typename VALUE, KEY NULL_KEY, typename KEY_OF, typename KEY_COMPARE, typename ALLOCATOR, BlockSizeType MIN_SIZE, BlockSizeType MAX_SIZE, typename SDS_TYPE>
class OverallocatedPmaEntry {
private:
    using ValueType = VALUE;
    using KeyType = KEY;
    using KeyOf = KEY_OF;
    using KeyCompare = KEY_COMPARE;
    using Allocator = ALLOCATOR;
    using AllocatorTraits = std::allocator_traits<Allocator>;

    static constexpr BlockSizeType MinSize = MIN_SIZE;
    static constexpr BlockSizeType MaxSize = MAX_SIZE;

    using BlockType = std::array<ValueType, MaxSize>;
    using BlockPointer = BlockType*;
    using PmaEntryType = OverallocatedPmaEntry<KeyType, ValueType, NULL_KEY, KeyOf, KeyCompare, Allocator, MinSize, MaxSize, SDS_TYPE>;

#ifndef NDEBUG
    //Used to set empty cells to zero in debug mode if there are only keys (no mapped type).
    static constexpr bool IsTrivialType = std::is_same_v<KeyType, ValueType>;
    static constexpr KeyType TRIVIAL_NULL_VALUE = NULL_KEY;
#endif

public:
    BlockPointer block;
    KeyType head;//The first element of the given block (NULL_KEY, iff block == nullptr)
    BlockSizeType size;//The number of elements in the given block (0, iff block == nullptr)

    [[no_unique_address]] KeyCompare comp;

    friend class BlockSparseArrayIterator<SDS_TYPE>;//Iterators can access the private data.

public:
    OverallocatedPmaEntry() : block(nullptr),
        head(NULL_KEY),
        size(0),
        comp() {}

    OverallocatedPmaEntry(BlockPointer b, const KeyType &v, BlockSizeType s) : block(b),
        head(v),
        size(s) {}

    void allocateBlock(Allocator &allocator) {
        AssertMsg(block == nullptr, "Allocating block that already exists.");
        block = reinterpret_cast<BlockPointer>(AllocatorTraits::allocate(allocator, MaxSize));
        AllocatorTraits::construct(allocator, block);
#ifndef NDEBUG
        if constexpr (IsTrivialType) {
            std::fill(block->begin(), block->end(), NULL_KEY);//If executed, the NULL_KEY acts as value here.
        }
#endif
    }

    void deleteBlock(Allocator &allocator) {
        AssertMsg(block != nullptr, "Deleting a block that does not exist.");
        AllocatorTraits::destroy(allocator, block);
        block = nullptr;
    }

    ValueType get(BlockIndexType index) const {
        AssertMsg(block != nullptr, "Invalid block");
        return (*block)[index];
    }

    /**
     * Parameter searchStartIndex is used as hint to the linear search.
     * Returns (insertion successful?, index of the insertion).
     */
    std::pair<bool, BlockIndexType> insertWithIndex(ValueType e, BlockIndexType searchStartIndex = 0) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(size < MaxSize, "Inserting into block that is already full.");
        //Linear search to find the first element >= e
        const BlockIndexType insertionPosition = linearSearch(KeyOf::keyOfValue(e), searchStartIndex);
        if (KeyOf::keyOfValue(get(insertionPosition)) == KeyOf::keyOfValue(e)) {
            //Element is already there
            AssertMsg(size == 0 || head == KeyOf::keyOfValue(get(0)), "Wrong head");
            return std::make_pair(false, insertionPosition);
        }
        insertAtIndex(e, insertionPosition);
        if (insertionPosition == 0) updateHead();
        AssertMsg(size == 0 || head == KeyOf::keyOfValue(get(0)), "Wrong head");
        return std::make_pair(true, insertionPosition);
    }

    /**
     * Merges numberOfElements elements from the sorted batch into this block.
     * Optimized for a small numberOfElements.
     */
    void merge(ValueType *batch, BlockSizeType numberOfElements) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(numberOfElements > 0, "No elements to merge.");
        //Do first insertion here to keep updateHead() outside the loop
        BlockIndexType insertionPosition = linearSearch(KeyOf::keyOfValue(batch[0]), 0);
        if (KeyOf::keyOfValue(get(insertionPosition)) != KeyOf::keyOfValue(batch[0])) {
            insertAtIndex(batch[0], insertionPosition);
            if (insertionPosition == 0) updateHead();//Only necessary if no duplicate and inserted at index 0.
        }
        for (BlockIndexType i = 1; i < numberOfElements; ++i) {//Loop starts at 1!
            //Search the insertion position (starting from the last)
            insertionPosition = linearSearch(KeyOf::keyOfValue(batch[i]), insertionPosition);
            if (KeyOf::keyOfValue(get(insertionPosition)) != KeyOf::keyOfValue(batch[i])) {
                insertAtIndex(batch[i], insertionPosition);
            }
        }
        AssertMsg(size == 0 || head == KeyOf::keyOfValue(get(0)), "Wrong head");
    }

    /**
     * Merges numberOfElements elements from the sorted batch into this block.
     * Optimized for a high numberOfElements.
     */
    void mergeFaster(ValueType *batch, BlockSizeType numberOfElements) {
#pragma GCC diagnostic ignored "-Wconversion" //Avoid warnings from signed/unsigned indices
        AssertMsg(block != nullptr, "Invalid block");
        BlockSizeType numberOfWrites = 0;
        size_t dupl = 0;
        SignedBlockIndexType blockEnd = size - 1;
        SignedBlockIndexType batchEnd = numberOfElements - 1;
        SignedBlockIndexType writeIndex = size + numberOfElements - 1;
        KeyType lastWrittenKey = NULL_KEY;
        while (blockEnd >= 0 && batchEnd >= 0) {
            //Write the larger element to the output
            AssertMsg(writeIndex >= 0 && (!IsTrivialType || empty(writeIndex)), "Write index invalid");
            //Write the larger element, or the block element (that already exists) if they are equal.
            if (comp(KeyOf::keyOfValue(get(blockEnd)), KeyOf::keyOfValue(batch[batchEnd]))) {
                if (KeyOf::keyOfValue(batch[batchEnd]) != lastWrittenKey) {
                    while (batchEnd > 0 && KeyOf::keyOfValue(batch[batchEnd]) == KeyOf::keyOfValue(batch[batchEnd - 1])) {
                        --batchEnd;//Go to the first occurrence of the key in the batch, only insert that one
                    }
                    (*block)[writeIndex] = batch[batchEnd];
                    ++numberOfWrites;
                    lastWrittenKey = KeyOf::keyOfValue(batch[batchEnd]);
                    --writeIndex;
                } else {
                    ++dupl;
                }
                --batchEnd;
            } else {
                if (KeyOf::keyOfValue(get(blockEnd)) != lastWrittenKey) {
                    (*block)[writeIndex] = get(blockEnd);
                    lastWrittenKey = KeyOf::keyOfValue(get(blockEnd));
                    --writeIndex;
                    ++numberOfWrites;
                } else {
                    ++dupl;
                }
#ifndef NDEBUG
                if constexpr (IsTrivialType) (*block)[blockEnd] = TRIVIAL_NULL_VALUE;
#endif
                --blockEnd;
            }
        }
        //Insert remaining parts of batch (nothing to do if blockEnd > 0: elements are already there)
        while (batchEnd >= 0) {
            AssertMsg(writeIndex >= 0 && (!IsTrivialType || empty(writeIndex)), "Write index invalid");
            if (KeyOf::keyOfValue(batch[batchEnd]) != lastWrittenKey) {
                (*block)[writeIndex] = batch[batchEnd];
                ++numberOfWrites;
                --writeIndex;
                lastWrittenKey = KeyOf::keyOfValue(batch[batchEnd]);
            } else {
                ++dupl;
            }
            --batchEnd;
        }
        if (blockEnd >= 0 && KeyOf::keyOfValue(get(blockEnd)) == lastWrittenKey) {
            ++dupl;
        }
        //Shift elements forward in the array
        const BlockSizeType emptyRegionStart = blockEnd + 1 - (blockEnd >= 0 && KeyOf::keyOfValue(get(blockEnd)) == lastWrittenKey ? 1 : 0);//Overwrite the potential duplicate as well.
        BlockSizeType numberOfDuplicates = size + numberOfElements - (blockEnd + 1) - numberOfWrites;//total number of operations - number of actual writes
        AssertMsg(dupl == numberOfDuplicates, "Wrong number of duplicates");
        if (blockEnd >= 0 && KeyOf::keyOfValue(get(blockEnd)) == lastWrittenKey) ++numberOfDuplicates;//There is a duplicate with the part of the block we did not rewrite
        if (numberOfDuplicates > 0) {
            for (BlockIndexType i = 0; i < numberOfWrites; ++i) {
                AssertMsg(KeyOf::keyOfValue(get(emptyRegionStart + i + numberOfDuplicates)) != 0, "Writing 0");
                (*block)[emptyRegionStart + i] = get(emptyRegionStart + i + numberOfDuplicates);
#ifndef NDEBUG
                if constexpr (IsTrivialType) (*block)[emptyRegionStart + i + numberOfDuplicates] = TRIVIAL_NULL_VALUE;
#endif
            }
        }
        if (writeIndex >= emptyRegionStart) writeIndex -= numberOfDuplicates;//Update the shifted write index for correct update of the head
        size = size + numberOfElements - numberOfDuplicates;
        if (writeIndex + 1 == 0) updateHead();//Index 0 was written (with a batch element, in that case)
#pragma GCC diagnostic pop
        AssertMsg(size == 0 || head == KeyOf::keyOfValue(get(0)), "Wrong head");
    }

    /**
     * Returns the new block of elements that was split to the right.
     */
    PmaEntryType split(Allocator &allocator) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(size == MaxSize, "Splitting block that is not full.");
        PmaEntryType rightHalf{};
        rightHalf.allocateBlock(allocator);
        const BlockIndexType splitPosition = MaxSize / 2;
        //Write elements to rightHalf (and null them here if debug)
        std::copy(block->begin() + splitPosition, block->end(), rightHalf.block->begin());
#ifndef NDEBUG
        for (BlockIndexType i = splitPosition; i < MaxSize; ++i) {
            if constexpr (IsTrivialType) block->operator[](i) = TRIVIAL_NULL_VALUE;
        }
#endif
        size = splitPosition;
        rightHalf.size = splitPosition;
        rightHalf.updateHead();//This block's head has not changed.
        AssertMsg(size == 0 || head == KeyOf::keyOfValue(get(0)), "Wrong head");
        return rightHalf;
    }

    BlockIndexType findIndex(KeyType e) const {
        AssertMsg(block != nullptr, "Invalid block");
        return linearSearch(e);
    }

    bool contains(KeyType e) const {
        AssertMsg(block != nullptr, "Invalid block");
        BlockIndexType pos = linearSearch(e);
        return pos < size && e == KeyOf::keyOfValue(get(pos));
    }

    /**
     * Removes an element from the block.
     */
    bool remove(ValueType e) {
        AssertMsg(block != nullptr, "Invalid block");
        BlockIndexType pos = linearSearch(e);
        if (pos < size && KeyOf::keyOfValue(get(pos)) == KeyOf::keyOfValue(e)) {
            moveForward(pos);
            --size;
            updateHead();
            return true;
        }
        return false;
    }

    /**
     * Merges all element from other into this block.
     * All elements from other must be smaller than all elements from this.
     * Does not change the elements in other.
     */
    inline void mergeSmaller(const PmaEntryType &other) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(other.block != nullptr, "Invalid block in other");
        //Move elements from this to the back
        for (BlockIndexType i = size; i-- != 0;) {//! This is a loop from size-1 downto 0
            (*block)[other.size + i] = get(i);
#ifndef NDEBUG
            if constexpr (IsTrivialType) (*block)[i] = TRIVIAL_NULL_VALUE;
#endif
        }
        //Insert elements from other
        for (BlockIndexType i = 0; i < other.size; ++i) {
            (*block)[i] = other.get(i);
        }
        size += other.size;
        updateHead();
    }

    /**
     * Moves the necessary number of elements from right to this block
     * to ensure that both have a valid number of elements.
     * Requires that both blocks together have enough elements.
     */
    void steal(PmaEntryType &right) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(right.block != nullptr, "Invalid block");
        AssertMsg(size > 0, "Empty block");
        AssertMsg(size + right.size >= 2 * MinSize, "Not enough elements for two blocks!");
        const BlockSizeType numberOfElementsNeeded = MinSize - size;
        //Copy the smallest elements from right to the end of this block
        std::copy(right.block->begin(), right.block->begin() + numberOfElementsNeeded, block->begin() + size);
#ifndef NDEBUG
        for (BlockIndexType i = 0; i < numberOfElementsNeeded; ++i) {
            if constexpr (IsTrivialType) right.block->operator[](i) = TRIVIAL_NULL_VALUE;
        }
#endif
        size += numberOfElementsNeeded;
        //In right, move the remaining elements to the front.
        std::copy(right.block->begin() + numberOfElementsNeeded, right.block->begin() + right.size, right.block->begin());
#ifndef NDEBUG
        for (BlockIndexType i = right.size - numberOfElementsNeeded; i < right.size; ++i) {
            if constexpr (IsTrivialType) right.block->operator[](i) = TRIVIAL_NULL_VALUE;
        }
#endif
        right.size -= numberOfElementsNeeded;
        AssertMsg(size >= MinSize, "Size still too small.");
        AssertMsg(right.size >= MinSize, "Other block's size got too small.");
        right.updateHead();
    }


    /**
     * Moves the necessary number of elements from left to this block
     * to ensure that both have a valid number of elements.
     * Requires that both blocks together have enough elements.
     */
    void stealFromSmaller(PmaEntryType &left) {
        AssertMsg(block != nullptr, "Invalid block");
        AssertMsg(left.block != nullptr, "Invalid block");
        AssertMsg(size > 0, "Empty block");
        AssertMsg(size + left.size >= 2 * MinSize, "Not enough elements for two blocks!");
        const BlockSizeType numberOfElementsNeeded = MinSize - size;
        //In this block, shift elements to the right.
        std::copy_backward(block->begin(), block->begin() + size, block->begin() + size + numberOfElementsNeeded);
#ifndef NDEBUG
        for (BlockIndexType i = 0; i < numberOfElementsNeeded; ++i) {
            if constexpr (IsTrivialType) block->operator[](i) = TRIVIAL_NULL_VALUE;
        }
#endif
        //Copy the smallest elements from the end of left to the start of this block
        std::copy(left.block->begin() + left.size - numberOfElementsNeeded, left.block->begin() + left.size, block->begin());
#ifndef NDEBUG
        for (BlockIndexType i = left.size - numberOfElementsNeeded; i < left.size; ++i) {
            if constexpr (IsTrivialType) left.block->operator[](i) = TRIVIAL_NULL_VALUE;
        }
#endif
        size += numberOfElementsNeeded;
        left.size -= numberOfElementsNeeded;
        AssertMsg(size >= MinSize, "Size still too small.");
        AssertMsg(left.size >= MinSize, "Other block's size got too small.");
        updateHead();//head of left can not have changed.
    }

    void removeLast() {
#ifndef NDEBUG
            if constexpr (IsTrivialType) block->operator[](size - 1) = TRIVIAL_NULL_VALUE;
#endif
        --size;
    }

private:
    /**
     * Updates the size but not the head.
     */
    void insertAtIndex(ValueType e, BlockIndexType index) {
        AssertMsg(block != nullptr, "Invalid block");
        makeFree(index);
        (*block)[index] = e;
        ++size;
    }

    void makeFree(BlockIndexType index) {
        BlockIndexType shift = index;
        ValueType carry;
#ifndef NDEBUG
        if constexpr (IsTrivialType) carry = TRIVIAL_NULL_VALUE;
#endif
        while (shift <= size) {
            std::swap((*block)[shift], carry);
            ++shift;
        }
        AssertMsg(!IsTrivialType || empty(index), "Make free did not make free.");
    }

    void moveForward(BlockIndexType index) {
        BlockIndexType shift = index;
        while (shift + 1 < size) {
            (*block)[shift] = get(shift + 1);
            ++shift;
        }
#ifndef NDEBUG
        if constexpr (IsTrivialType) (*block)[size - 1] = TRIVIAL_NULL_VALUE;
#endif
    }

    BlockIndexType linearSearch(KeyType e, BlockIndexType searchStart = 0) const {
        BlockIndexType insertionPosition = searchStart;
        while (insertionPosition < size && comp(KeyOf::keyOfValue(get(insertionPosition)), e)) ++insertionPosition;
        return insertionPosition;
    }

    bool empty(BlockIndexType index) const {
        return KeyOf::keyOfValue(get(index)) == NULL_KEY;
    }

    void updateHead() {
        head = KeyOf::keyOfValue(get(0));
    }

public:
    bool isEmpty() const {
        return block == nullptr;
    }

    friend std::ostream& operator<<(std::ostream& out, const OverallocatedPmaEntry& entry) {
        for (ValueType val : (*entry.block)) out << KeyOf::keyOfValue(val) << " ";
        return out;
    }

    bool isValid() const {
        //AssertMsg(IsTrivialType, "Validation only works for trivial types, where empty cells are NULL_KEY.");
#ifndef NDEBUG
        if (!IsTrivialType) return true;
        bool values = true;
        for (BlockIndexType i = 0; i < MaxSize; ++i) {
            if (empty(i)) {
                values = false;
            } else {
                AssertMsg(values, "Saw value after non-value in block!");
                if (!values) return false;
            }
        }
        KeyType current = NULL_KEY;
        for (BlockIndexType i = 0; i < MaxSize; ++i) {
            if (empty(i)) break;
            AssertMsg(comp(current, KeyOf::keyOfValue(get(i))), "Block values not ordered in block " << (*this));
            if (comp(KeyOf::keyOfValue(get(i)), current) || KeyOf::keyOfValue(get(i)) == current) return false; // Not ordered or duplicate
            current = KeyOf::keyOfValue(get(i));
        }
        for (BlockIndexType i = 0; i < size; ++i) {
            AssertMsg(!empty(i), "Wrong size!");
            if (empty(i)) return false;
        }
        for (BlockIndexType i = size; i < MaxSize; ++i) {
            AssertMsg(empty(i), "Wrong size!");
            if (!empty(i)) return false;
        }
        return true;
#else
        std::cout << "Warning: block validation only active in debug mode." << std::endl;
        return true;
#endif
    }
};
}
