#pragma once

#include <vector>

#include <bulk_operations_data_structure.h>

template <typename TBtree>
struct  TBaseSplit
{
public:
    using TKeyType = typename TBtree::key_type;
    using TPairType = typename TBtree::pair_type;

    const static TKeyType MinSep;



    const static TKeyType MaxSep;

};

template <typename TBtree>
const typename TBtree::key_type TBaseSplit<TBtree>::MinSep = std::numeric_limits<TKeyType>::min();

template <typename TBtree>
const typename TBtree::key_type TBaseSplit<TBtree>::MaxSep = std::numeric_limits<TKeyType>::max();

template <typename TBtree>
struct TSequentialSplit : TBaseSplit<TBtree>
{
public:
    using TBase = TBaseSplit<TBtree>;
    using typename TBase::TKeyType;
    using typename TBase::TPairType;

    static std::atomic_uint_fast8_t Counter;
private:


    TBtree& Tree;
    TMetaTree<TBtree>* SplittedTrees;
    const std::vector<TKeyType>& Data;
    size_t Id;
    uint32_t TreesNumber;

public:
    TSequentialSplit(TBtree& tree, TMetaTree<TBtree>* splittedTrees, size_t id, const std::vector<TKeyType>& data,
                     uint32_t treesNumber)
            :   Tree(tree)
            ,   SplittedTrees(splittedTrees)
            ,   Data(data)
            ,   Id(id)
            ,   TreesNumber(treesNumber)
    {
    }

    static void Reset()
    {
        Counter = 0;
    }

    void operator() ()
    {
        if (Id != 0)
        {
            while (Counter < TreesNumber);
            return;
        }

        size_t sepIndex = 0;
        size_t step = Data.size() / TreesNumber;

        for (size_t treeInd = 0; treeInd + 1 < TreesNumber; ++treeInd)
        {
            sepIndex += step;
            TKeyType curSep = Data[sepIndex];


            TBtree newTree;
            Tree.split(*SplittedTrees[treeInd].Tree, curSep, newTree);

            Tree.swap(newTree);
            ++Counter;

        }
        (*SplittedTrees[TreesNumber - 1].Tree).swap(Tree);

        ++Counter;
    }
};

template <typename TBtree>
std::atomic_uint_fast8_t TSequentialSplit<TBtree>::Counter(0);

template <typename TBtree>
struct TParallelSplit : public TBaseSplit<TBtree>
{
public:
    using TBase = TBaseSplit<TBtree>;
    using typename TBase::TKeyType;
    using typename TBase::TPairType;
    using TBase::MinSep;
    using TBase::MaxSep;
private:
    using TNode = typename TBtree::node;
    using TInner = typename TBtree::inner_node;
    using TLeaf = typename TBtree::leaf_node;

private:
    static std::atomic_uint_fast8_t Counter;

    TBtree& Tree;
    TBtree& SplittedTree;
    TKeyType CurSep, PrevSep;
    uint16_t PrevSlot, Slot;
    uint32_t TreesNumber;
    std::vector<TNode*> NodesToDelete;

public:
    TParallelSplit(TBtree& tree, TMetaTree<TBtree>* splittedTrees, size_t id, const std::vector<TKeyType>& data,
                   uint32_t treesNumber)
            :   Tree(tree)
            ,   SplittedTree(*splittedTrees[id].Tree)
            ,   PrevSlot(0)
            ,   Slot(0)
            ,   TreesNumber(treesNumber)
    {
        NodesToDelete.reserve(Tree.height());

        size_t bulkSize = data.size() / TreesNumber;
        PrevSep = id > 0 ? data[bulkSize * id] : MinSep;
        CurSep = id + 1 < TreesNumber ? data[bulkSize * (id + 1)] : MaxSep;
    }

    TParallelSplit(TBtree& tree, TBtree& splittedTree, TKeyType prevSep, TKeyType curSep, uint32_t treesNumber)
            :   Tree(tree)
            ,   SplittedTree(splittedTree)
            ,   CurSep(curSep)
            ,   PrevSep(prevSep)
            ,   PrevSlot(0)
            ,   Slot(0)
            ,   TreesNumber(treesNumber)
    {
        NodesToDelete.reserve(Tree.height());
    }

    static void Reset()
    {
        Counter = 0;
    }

    void operator() ()
    {
        if (PrevSep >= CurSep)
        {
            ++Counter;
            return;
        }
        // Search for node where§
        // previous separator and current
        // separator are in different subtrees
        TNode* splitNode = SearchSplitNode(Tree.m_root);

        // Go down from splitNode node,
        // join right subtrees
        // in left child of PrevSep's slot
        // and join left subtrees
        // in left child of CurSep's slot
        StartSplit(splitNode, SplittedTree);

        ++Counter;

        DeleteNodes();

        if (PrevSep == MinSep)
            Tree.m_root = nullptr;
    }

    static std::atomic_uint_fast8_t& GetCounter()
    {
        return Counter;
    }

private:
    TInner* Inner(TNode* node)
    {
        return static_cast<TInner*>(node);
    }

    TLeaf* Leaf(TNode* node)
    {
        return static_cast<TLeaf*>(node);
    }

    TNode* SearchSplitNode(TNode* node)
    {
        if (PrevSep == MinSep)
            NodesToDelete.push_back(node);

        if (!node->isleafnode())
        {
            Slot = Tree.find_upper(Inner(node), CurSep);
            PrevSlot = Tree.find_upper(Inner(node), PrevSep);
        }
        else
        {
            Slot = Tree.find_upper(Leaf(node), CurSep);
            PrevSlot = Tree.find_upper(Leaf(node), PrevSep);
        }

        // Сheck if previous separotor is
        // in the other subtree. If PrevSep's slot
        // less than CurSep's slot then they are
        // in different subtrees
        if (PrevSlot < Slot)
            return node;
        else if (!node->isleafnode())
            return SearchSplitNode(Inner(node)->childid[Slot]);
        else
            return node;
    }

    void StartSplit(TNode* node, TBtree& resultTree)
    {
        if (!node->isleafnode())
        {
            auto inner = Inner(node);

            // Build central tree
            TNode* newRoot = nullptr;
            if (PrevSlot + 1 < Slot)
            {
                newRoot = Tree.allocate_inner(node->level);
                newRoot = CopyInner(inner, Inner(newRoot), PrevSlot + 1, Slot - 1);
            }

            resultTree.m_root = newRoot;

            TBtree rightTree;
            JoinLeftSubtrees(inner->childid[Slot], rightTree);

            // join right tree to central tree
            if (resultTree.height() >= rightTree.height())
                resultTree.join_greater(inner->slotkey[Slot - 1], rightTree);
            else
            {
                rightTree.join_less(inner->slotkey[Slot - 1], resultTree);
                resultTree.swap(rightTree);
            }


            // Join left tree to central tree
            TBtree leftTree;
            JoinRightSubtrees(inner->childid[PrevSlot], leftTree);

            if (resultTree.height() >= leftTree.height())
                resultTree.join_less(inner->slotkey[PrevSlot], leftTree);
            else
            {
                leftTree.join_greater(inner->slotkey[PrevSlot], resultTree);
                resultTree.swap(leftTree);
            }
        }
        else
        {
            auto leaf = Leaf(node);
            if (PrevSlot < Slot)
            {
                resultTree.m_root = Tree.allocate_leaf();
                CopyLeaf(leaf, Leaf(resultTree.m_root), PrevSlot, Slot);
            }
        }
    }

    // Guaranteed that PrevSep in other subtree
    void JoinLeftSubtrees(TNode* node, TBtree& resultTree)
    {
        NodesToDelete.push_back(node);

        if (!node->isleafnode())
        {
            auto inner = Inner(node);
            int slot = Tree.find_upper(inner, CurSep);
            TNode* newRoot = nullptr;
            if (slot > 0)
            {
                newRoot = Tree.allocate_inner(inner->level);
                newRoot = CopyInner(inner, Inner(newRoot), 0, slot - 1);
            }
            resultTree.m_root = newRoot;

            TBtree bottomTree;
            JoinLeftSubtrees(inner->childid[slot], bottomTree);

            if (slot > 0)
                resultTree.join_greater(inner->slotkey[slot - 1], bottomTree);
            else
                resultTree.swap(bottomTree);
        }
        else
        {
            auto leaf = Leaf(node);
            int slot = Tree.find_upper(leaf, CurSep);
            TNode* newRoot = nullptr;
            if (slot != 0)
            {
                newRoot = Tree.allocate_leaf();
                Leaf(newRoot)->prevleaf = nullptr;
                Leaf(newRoot)->nextleaf = nullptr;
                CopyLeaf(leaf, Leaf(newRoot), 0, slot);
            }
            resultTree.m_root = newRoot;
        }
    }

    // Guaranteed that CurSep in other subtree
    void JoinRightSubtrees(TNode* node, TBtree& resultTree)
    {
        if (PrevSep == MinSep)
            NodesToDelete.push_back(node);
        if (!node->isleafnode())
        {
            auto inner = Inner(node);
            int slot = Tree.find_upper(inner, PrevSep);
            TNode* newRoot = nullptr;
            if (slot < node->slotuse)
            {
                newRoot = Tree.allocate_inner(inner->level);
                newRoot = CopyInner(inner, Inner(newRoot), slot + 1, node->slotuse);
            }
            resultTree.m_root = newRoot;

            TBtree bottomTree;
            JoinRightSubtrees(inner->childid[slot], bottomTree);

            if (slot < node->slotuse)
                resultTree.join_less(inner->slotkey[slot], bottomTree);
            else
                resultTree.swap(bottomTree);
        }
        else
        {
            int slot = Tree.find_upper(Leaf(node), PrevSep);
            TNode* newRoot = nullptr;
            if (slot != node->slotuse)
            {
                newRoot = Tree.allocate_leaf();
                Leaf(newRoot)->prevleaf = nullptr;
                Leaf(newRoot)->nextleaf = nullptr;
                CopyLeaf(Leaf(node), Leaf(newRoot), slot, node->slotuse);
            }
            resultTree.m_root = newRoot;
        }
    }

    TNode* CopyInner(TInner* src, TInner* dst, uint16_t begin, uint16_t end)
    {
        std::copy(src->slotkey + begin, src->slotkey + end, dst->slotkey);
        std::copy(src->childid + begin, src->childid + end + 1, dst->childid);
        dst->slotuse = end - begin;

        if (dst->slotuse == 0)
        {
            TNode* node = dst->childid[0];
            Tree.free_node(dst);
            return node;
        }
        return dst;
    }

    void CopyLeaf(TLeaf* src, TLeaf* dst, uint16_t begin, uint16_t end)
    {
        std::copy(src->slotkey + begin, src->slotkey + end, dst->slotkey);
        if (!Tree.used_as_set)
            std::copy(src->slotdata + begin, src->slotdata + end, dst->slotdata);
        dst->slotuse = end - begin;
    }

    void DeleteNodes()
    {
        while (Counter != TreesNumber);

        for (auto node : NodesToDelete)
            Tree.free_node(node);
    }
};

template <typename TBtree>
std::atomic_uint_fast8_t TParallelSplit<TBtree>::Counter(0);