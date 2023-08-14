#pragma once

#include "typedefs.hpp"

namespace DataStructures {
template<typename SDS_TYPE>
class BlockSparseArrayIterator {
private:
    using SdsType = SDS_TYPE;
    using IteratorType = BlockSparseArrayIterator<SdsType>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = int;
    using value_type = SdsType::ValueType;
    using pointer = SdsType::Pointer;
    using reference = SdsType::Reference;

private:
    const SdsType *sds_;//Pointer to the data structure we are iterating
    IndexType blockIndex_;
    IndexType elementIndex_;

public:
    BlockSparseArrayIterator(SdsType *sds, IndexType blockIndex, IndexType elementIndex) :
        sds_(sds),
        blockIndex_(blockIndex),
        elementIndex_(elementIndex) { }

    IteratorType operator++() {
        IteratorType i = *this;
        findNext();
        return i;
    }

    IteratorType operator++(int) {
        findNext();
        return *this;
    }

    IteratorType operator--() {
        IteratorType i = *this;
        findPrevious();
        return i;
    }

    IteratorType operator--(int) {
        findPrevious();
        return *this;
    }

    SdsType::Reference operator*() const {
        return sds_->pma[blockIndex_].block->operator[](elementIndex_);
    }

    SdsType::Pointer operator->() const {
        return sds_->pma[blockIndex_].block->operator[](elementIndex_);
    }

    bool operator==(const IteratorType& other) const {
        return sds_ == other.sds_ && blockIndex_ == other.blockIndex_ && elementIndex_ == other.elementIndex_;
    }

    bool operator!=(const IteratorType& other) const {
        return sds_ != other.sds_ || blockIndex_ != other.blockIndex_ || elementIndex_ != other.elementIndex_;
    }
private:
    void findNext() {
        if (elementIndex_ + 1 < sds_->blockSize(blockIndex_)) {
            ++elementIndex_;//There is another element in the block
        } else {
            //Find the next block
            ++blockIndex_;
            while (blockIndex_ < sds_->size && sds_->empty(blockIndex_)) ++blockIndex_;
            //And use its first element
            elementIndex_ = 0;
        }
    }

    void findPrevious() {
        if (elementIndex_ > 0) {
            --elementIndex_;//There is another element in the block
        } else {
            //Find the previous block
            --blockIndex_;
            while (blockIndex_ > 0 && sds_->empty(blockIndex_)) --blockIndex_;
            //And its last element
            elementIndex_ = sds_->block(blockIndex_)->size - 1;
        }
    }
};

template<typename SDS_TYPE>
class SparseArrayIterator {
private:
    using SdsType = SDS_TYPE;
    using IteratorType = SparseArrayIterator<SdsType>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = int;
    using value_type = SdsType::ValueType;
    using pointer = SdsType::Pointer;
    using reference = SdsType::Reference;

private:
    const SdsType *sds_;
    IndexType index_;

public:
    SparseArrayIterator(SdsType *sds, IndexType index) : sds_(sds), index_(index) { }

    IteratorType operator++() {
        IteratorType i = *this;
        findNext();
        return i;
    }

    IteratorType operator++(int) {
        findNext();
        return *this;
    }

    IteratorType operator--() {
        IteratorType i = *this;
        findPrevious();
        return i;
    }

    IteratorType operator--(int) {
        findPrevious();
        return *this;
    }

    SdsType::Reference operator*() const {
        return sds_->data[index_];
    }

    SdsType::Pointer operator->() const {
        return sds_->data[index_];
    }

    bool operator==(const IteratorType& other) const {
        return sds_ == other.sds_ && index_ == other.index_;
    }

    bool operator!=(const IteratorType& other) const {
        return sds_ != other.sds_ || index_ != other.index_;
    }
private:
    void findNext() {
        ++index_;
        while (index_ < sds_->activeEnd && sds_->empty(index_)) ++index_;
    }

    void findPrevious() {
        --index_;
        while (index_ > sds_->activeStart && sds_->empty(index_)) --index_;
    }
};
}