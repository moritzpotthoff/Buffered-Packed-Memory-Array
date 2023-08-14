#pragma once

#include <bit>
#include <vector>

#include "typedefs.hpp"

namespace DataStructures {
/**
 * Only used for old implementations
 */
template<typename T>
inline bool isZero(T value) {
    return (value == NULL_VALUE);
}

inline IndexType integerLog2(UnsignedSizeType value) {
    return static_cast<IndexType>(std::bit_width(value) - 1);
}

struct Interval {
    IndexType left;//inclusive
    IndexType right;//exclusive

    Interval() : left(0), right(0) {}

    Interval(IndexType l, IndexType r) : left(l), right(r) {}

    IndexType length() const {
        return right - left;
    }

    friend std::ostream& operator<<(std::ostream& out, const Interval& interval) {
        out << "[" << interval.left << ", " << interval.right << ")";
        return out;
    }

    bool operator==(const Interval& other) {
        return left == other.left && right == other.right;
    }

    bool contains(const IndexType index) const {
        return index >= left && index < right;
    }

    bool overlaps(const Interval &other) const {
        return cut(other) != Interval();
    }

    Interval cut(const Interval &other) const {
        Interval result = other;
        if (left > result.left) result.left = left;
        if (right < result.right) result.right = right;
        if (result.length() <= 0) {
            result.left = result.right = 0;
        }
        return result;
    }
};

enum TaskType {
    INSERTION,
    INVERSE_MERGE,
    DO_NOTHING,
    REBALANCE
};

struct RecountingNode {
public:
    IndexType index;
    bool needsRebalancing;

    RecountingNode(IndexType i, bool n) noexcept : index(i), needsRebalancing(n) {}

    RecountingNode() noexcept : index(-1), needsRebalancing(false) {}
public:
    friend std::ostream& operator<<(std::ostream& out, const RecountingNode& node) {
        out << "node index " << node.index << ", needs rebalancing: " << node.needsRebalancing;
        return out;
    }

    friend bool operator<(const RecountingNode &lhs, const RecountingNode &rhs) {
        return lhs.index < rhs.index
               || (lhs.index == rhs.index && lhs.needsRebalancing);
    }

    friend bool operator==(const RecountingNode &lhs, const RecountingNode &rhs) {
        return lhs.index == rhs.index;
    }
};

struct RebalancingNode {
public:
    IndexType level;
    IndexType index;

public:
    friend std::ostream& operator<<(std::ostream& out, const RebalancingNode& node) {
        out << "level " << node.level << ", node " << node.index;
        return out;
    }

    friend bool operator ==(const RebalancingNode& lhs, const RebalancingNode& rhs) {
        return lhs.level == rhs.level && lhs.index == rhs.index;
    }
};

struct RebalancingTask {
public:
    Interval pmaRange;
    SizeType numberOfBlocks;
    RebalancingNode rootNode;

public:
    friend std::ostream& operator<<(std::ostream& out, const RebalancingTask& task) {
        out << "Root node " << task.rootNode << ", range " << task.pmaRange << ", #blocks " << task.numberOfBlocks;
        return out;
    }
};

/**
 * Struct that points to a block in a pma that was inserted to.
 * If bufferIndex == -1, points to pma[pmaIndex].
 * Otherwise, points to bufferArray[bufferIndex] (absolute indices!).
 */
struct ExtendedPmaIndex {
public:
    IndexType pmaIndex;
    IndexType bufferIndex;

public:
    ExtendedPmaIndex() :
        pmaIndex(0),
        bufferIndex(-1) {}

    ExtendedPmaIndex(IndexType p, IndexType b) :
            pmaIndex(p),
            bufferIndex(b) {}

    friend std::ostream& operator<<(std::ostream& out, const ExtendedPmaIndex& extendedIndex) {
        if (extendedIndex.bufferIndex == -1) {
            out << "block index " << extendedIndex.pmaIndex << ", no buffer block";
        } else {
            out << "block index " << extendedIndex.pmaIndex << ", buffer block index " << extendedIndex.bufferIndex;
        }
        return out;
    }

    friend bool operator <(const ExtendedPmaIndex &left, const ExtendedPmaIndex &right) noexcept {
        return left.pmaIndex < right.pmaIndex || (left.pmaIndex == right.pmaIndex && left.bufferIndex < right.bufferIndex);
    }

};

struct PartialRebalancingTask {
public:
    ExtendedPmaIndex blockStart;
    ExtendedPmaIndex blockEnd;
    IndexType outputOffset;

public:
    PartialRebalancingTask() :
        blockStart(),
        blockEnd(),
        outputOffset(0) {}


    PartialRebalancingTask(ExtendedPmaIndex start, ExtendedPmaIndex end, IndexType offset) :
        blockStart(start),
        blockEnd(end),
        outputOffset(offset) {}

    friend std::ostream& operator<<(std::ostream& out, const PartialRebalancingTask& task) {
        out << "Start " << task.blockStart << ", end " << task.blockEnd << ", with output offset " << task.outputOffset;
        return out;
    }
};

/**
 * All the information needed to construct and execute a task,
 * i.e., the insertion of a range of the batch into a range of the PMA.
 */
struct alignas(64) TaskInfo {
public:
    IndexType blockForBatchSplitter;//The index of the block where this task's splitter from the batch must be inserted
    IndexType firstBatchElement;//The index of the first element in the batch that must be inserted into the block of blockForBatchSplitter
    IndexType requiredWidth;//The maximum width required to execute this task
    IndexType startIndex;//The start index of this task
    TaskType type;
    UnsignedSizeType mergedTasks;//number of tasks to the left that were merged into this during distribution. Later: Number of subtasks in this merge group
    UnsignedSizeType mergedTaskIndex;//Index of the subtask in its merge group

    SizeType numberOfBlocksWritten;//Actual number of blocks written during the task execution, only used during out-of-place executions.
    //After insertion, for the rebalancing phase computations
    SizeType numberOfRebalancingTasks;
    SizeType nodesToRecount;

    //During rebalancing
    Interval rebalancingBlockRange;
    Interval rebalancingTaskRange;
    PartialRebalancingTask firstRebalancingTask;
    PartialRebalancingTask lastRebalancingTask;

public:
    friend std::ostream& operator<<(std::ostream& out, const TaskInfo& task) {
        out << "startBlock=" << task.blockForBatchSplitter
            << ", firstBatchElement=" << task.firstBatchElement
            << ", requiredWidth=" << task.requiredWidth
            << ", writeStartIndex=" << task.startIndex;
        return out;
    }
};
}