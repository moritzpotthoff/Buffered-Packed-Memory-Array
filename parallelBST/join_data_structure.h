#pragma once

#include "bulk_operations_data_structure.h"
#include "tree_joiner.h"
#include "memory.h"

template <typename TBtree>
struct TSequentialJoin
{
private:
    static uint8_t TasksNumber;
    static std::atomic_uint_fast8_t CounterJoin;

public:
    void operator() (TBtree& tree, TMetaTree<TBtree> trees[], size_t id, size_t treesNumber)
    {
        if (id != 0)
            return;

        tree.swap(*trees[0].Tree);

        for (size_t i = 1; i < treesNumber; ++i)
        {
            while (!trees[i].InsertPhaseEnd);

            typename TBtree::key_type key = trees[i - 1].Key;

            TBtree* otherTree = trees[i].Tree;

            if (otherTree->empty())
            {
                trees[i].Key = trees[i - 1].Key;
                continue;
            }

            if (tree.empty())
            {
                tree.swap(*otherTree);
                continue;
            }


            if (tree.height() >= otherTree->height())
                tree.join_greater(key, *otherTree);
            else
            {
                otherTree->join_less(key, tree);
                tree.swap(*otherTree);
            }
        }
    }

#ifdef JOIN_TEST
    static void Init(size_t size)
    {
        TasksNumber = size;
    }
#else
    static void Init(size_t)
    {}
#endif

    static void Reset()
    {
#ifdef JOIN_TEST
        CounterJoin = 0;
#endif
    }

    void AddTree(size_t, TBtree*)
    {}

    void InitTreeAndWait(size_t)
    {
#ifdef JOIN_TEST
        ++CounterJoin;
        while (CounterJoin != TasksNumber);
#endif
    }

    void GetResult(size_t, TBtree*)
    {}
};

#ifdef JOIN_TEST
template <typename TBtree>
std::atomic_uint_fast8_t TSequentialJoin<TBtree>::CounterJoin(0);

template <typename TBtree>
uint8_t TSequentialJoin<TBtree>::TasksNumber = 0;
#endif

template <typename TBtree>
struct TParallelJoin
{
    static TTreeJoiner<TBtree> Joiner;
    typename TTreeJoiner<TBtree>::TIterator Iter;
    static std::atomic_uint_fast8_t Counter;
    static uint8_t TasksNumber;
    static std::atomic_uint_fast8_t CounterJoin;

    static void Init(size_t size)
    {
        TasksNumber = size;
        Joiner.Reset(TasksNumber);
    }

    static void Reset()
    {
        Counter = 0;
        CounterJoin = 0;
    }

    void AddTree(size_t id, TBtree* tree)
    {
        Joiner.AddTree(id, tree);
    }

    void InitTreeAndWait(size_t id)
    {
        Iter = Joiner.begin() + id;
        Iter->Init();

        ++CounterJoin;
        while (CounterJoin != TasksNumber);
    }

    void GetResult(size_t id, TBtree* tree)
    {
        ++Counter;
        if (id == 0)
        {
            while (Counter != TasksNumber);
            tree->swap(*Joiner.begin()->Tree);
        }
    }
    void operator() (TBtree&, TMetaTree<TBtree>[], size_t, size_t)
    {
        StartJoiner(Joiner, Iter);
    }

    void operator() ()
    {
        StartJoiner(Joiner, Iter);
    }
};

template <typename TBtree>
TTreeJoiner<TBtree> TParallelJoin<TBtree>::Joiner(0);

template <typename TBtree>
std::atomic_uint_fast8_t TParallelJoin<TBtree>::Counter(0);

template <typename TBtree>
uint8_t TParallelJoin<TBtree>::TasksNumber = 0;

template <typename TBtree>
std::atomic_uint_fast8_t TParallelJoin<TBtree>::CounterJoin(0);

template <typename TBtree>
struct TParallelPairwiseJoin
{
private:
#ifdef JOIN_TEST
    static uint8_t TasksNumber;
    static std::atomic_uint_fast8_t CounterJoin;
#endif

public:
    using TKeyType = typename TBtree::key_type;

    void operator() (TBtree& tree, TMetaTree<TBtree> trees[], size_t id, size_t treesNumber)
    {
        if (id & 1)
        {
            trees[id].Done = true;
            return;
        }

        uint32_t offset = 1;
        uint32_t newId = id;
        while (id + offset <= treesNumber)
        {
            TKeyType key = trees[id].Key;

            uint32_t otherTreeId = id + offset;


            TBtree* newTree;
            TKeyType newKey;
            if (otherTreeId < treesNumber)
            {
                // Wait while neighbor's insertion phase is
                // not finished
                while (!trees[otherTreeId].Done);

                if (trees[otherTreeId].Tree.load()->empty())
                {
                    newKey = key;
                    newTree = trees[id].Tree;
                }
                else
                {
                    newKey = trees[otherTreeId].Key;
                    newTree = Join(trees[id].Tree, key, trees[otherTreeId].Tree);
                }
            }
            else
            {
                newKey = key;
                newTree = trees[id].Tree;
            }

            trees[id].Key = newKey;
            trees[id].Tree = newTree;

            newId >>= 1;
            if (newId & 1)
                break;

            offset <<= 1;
        }

        trees[id].Done = true;

        if (id == 0)
            tree.swap(*trees[0].Tree);
    }

#ifdef JOIN_TEST
    static void Init(size_t size)
    {
        TasksNumber = size;
    }
#else
    static void Init(size_t)
    {}
#endif

    static void Reset()
    {
#ifdef JOIN_TEST
        CounterJoin = 0;
#endif
    }

    void AddTree(size_t, TBtree*)
    {}

    void InitTreeAndWait(size_t)
    {
#ifdef JOIN_TEST
        ++CounterJoin;
        while (CounterJoin != TasksNumber);
#endif
    }

    void GetResult(size_t, TBtree*)
    {}

private:
    TBtree* Join(std::atomic<TBtree*>& tree, const TKeyType& key, std::atomic<TBtree*>& otherTree)
    {
        if (tree.load()->height() >= otherTree.load()->height())
        {
            tree.load()->join_greater(key, *otherTree);
            return tree;
        }
        else
        {
            otherTree.load()->join_less(key, *tree);
            TBtree* res = otherTree;
            otherTree.store(tree);
            return res;
        }

    }
};

#ifdef JOIN_TEST
template <typename TBtree>
std::atomic_uint_fast8_t TParallelPairwiseJoin<TBtree>::CounterJoin(0);

template <typename TBtree>
uint8_t TParallelPairwiseJoin<TBtree>::TasksNumber = 0;
#endif