#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <thread>
#include <vector>

#include <fast_list.h>
#include <memory.h>
#include <numeric.h>
#include <thread_pool.h>

#include <sys/syscall.h>

static size_t ThreadsNumber = 0;

static size_t GlobalIter = 0;

template <typename T>
struct TAtomicWrapper;

template <typename T>
std::ostream& operator<< (std::ostream& out, const TAtomicWrapper<T>& atomic);

template <typename T>
struct TAtomicWrapper
{
    std::atomic<T> Atomic;

    inline TAtomicWrapper(T value = T())
            :   Atomic(value)
    {}

    inline explicit TAtomicWrapper(const std::atomic<T>& atomic)
            :   Atomic(atomic.load(std::memory_order_acquire))
    {}

    inline explicit TAtomicWrapper(const TAtomicWrapper& other)
            :   Atomic(other.Atomic.load(std::memory_order_acquire))
    {}

    inline TAtomicWrapper& operator=(const TAtomicWrapper& other)
    {
        Atomic.store(other.Atomic.load(std::memory_order_acquire), std::memory_order_release);
        return *this;
    }

    inline TAtomicWrapper& operator=(const T& other)
    {
        Atomic.store(other, std::memory_order_release);
        return *this;
    }

    inline TAtomicWrapper& operator++()
    {
        Atomic.fetch_add(1, std::memory_order_acq_rel);
        return *this;
    }

    inline bool operator< (const T& other) const
    {
        return Atomic < other;
    }

    inline bool operator< (const TAtomicWrapper& other) const
    {
        return Atomic < other.Atomic;
    }

    inline bool operator> (const T& other) const
    {
        return Atomic > other;
    }

    inline bool operator> (const TAtomicWrapper& other) const
    {
        return Atomic > other.Atomic;
    }

    inline bool operator== (const T& value) const
    {
        return Atomic == value;
    }

    inline operator T () const
    {
        return Atomic.load(std::memory_order_acquire);
    }

    inline T Exchange(T desired, std::memory_order order)
    {
        return Atomic.exchange(desired, order);
    }

    inline bool Cas(T& expected, T desired, std::memory_order success = std::memory_order_seq_cst,
                    std::memory_order failure = std::memory_order_seq_cst)
    {
        return Atomic.compare_exchange_weak(expected, desired, success, failure);
    }

    friend std::ostream& operator<< <> (std::ostream& out, const TAtomicWrapper<T>& atomic);
};

template <typename T>
std::ostream& operator<< (std::ostream& out, const TAtomicWrapper<T>& atomic)
{
    return out << atomic.Atomic.load(std::memory_order_acquire);
}

template <typename TTree>
class TTreeJoiner
{
public:
    struct TMetaTree
    {
        constexpr static char Undefined = -1;
        constexpr static uint8_t MaxIter = std::numeric_limits<uint8_t>::max();
        constexpr static uint8_t MaxLevel = std::numeric_limits<uint8_t>::max();

        using TKey = typename TTree::key_type;
        using TSpine = typename TTree::spine;

        // Left spine ot the tree *Tree.
        // It is a std::vector which
        // pointers on left spine of tree.
        // First element of array points
        // to most left leaf.
    private:
        TSpine* LeftSpine;

        // Right spine
        TSpine* RightSpine;
    public:
        // pointer to corresponding search tree
        TTree* Tree;

        TKey MaxKey;

        // tree id
        uint8_t TreeID;

        // Number of iteration, need for sync between threads i.e.
        // thread finishes iteration and waits
        // while other threads will finish iteration
        volatile uint8_t Iteration;

        // Height of the tree
        volatile uint8_t Height;

        TAtomicWrapper<bool> Joining;

        // If tree inactive then we delete it
        volatile bool Active;

        TSpine& GetLeftSpine()
        {
            return *LeftSpine;
        }

        TSpine& GetRightSpine()
        {
            return *RightSpine;
        }

        uint8_t Dummy[30];

#if defined(JOINER_VERBOSE_FILE) || defined(VERBOSE_PHASE_TIME_FILE) || defined(JOINER_CHECK)
        std::shared_ptr<std::ofstream> Flog;
        std::shared_ptr<std::mutex> LogMutex;
#endif

        explicit TMetaTree(TTree* tree = nullptr, size_t treeID = 0)
                    :   Tree(tree)
                    ,   MaxKey(std::numeric_limits<TKey>::min())
                    ,   TreeID(treeID)
                    ,   Iteration(0)
                    ,   Height(Tree ? Tree->height() : MaxLevel)
                    ,   Joining(false)
                    ,   Active(true)
        {
            LeftSpine = new TSpine;
            RightSpine = new TSpine;
        }

        TMetaTree(const TMetaTree& metaTree)
                :   Tree(metaTree.Tree)
                ,   MaxKey(metaTree.MaxKey)
                ,   TreeID(metaTree.TreeID)
                ,   Iteration(metaTree.Iteration)
                ,   Height(metaTree.Height)
                ,   Joining(metaTree.Joining)
                ,   Active(metaTree.Active)
        {
            LeftSpine = new TSpine;
            RightSpine = new TSpine;
        }

        ~TMetaTree()
        {
            delete LeftSpine;
            delete RightSpine;
            //delete[] Randoms;
        }

#if defined(JOINER_VERBOSE_FILE) || defined(VERBOSE_PHASE_TIME_FILE) || defined(JOINER_CHECK)
        std::ofstream& Log()
        {
            return *Flog;
        }

        std::mutex& Locker()
        {
            return *LogMutex;
        }
#endif

        void Init()
        {
            Iteration = 0;
            Joining = false;
            Active = true;
            LeftSpine->clear();
            RightSpine->clear();

            Tree->build_spines(*LeftSpine, *RightSpine);
            MaxKey = Tree->max_key_slow();
            Height = Tree ? Tree->height() : MaxLevel;
        }

        void JoinLess(TMetaTree& otherTree)
        {
            // Necessary: consider the situation when we have three trees.
            // Central tree is the highes. Then Its left and right neighbours
            // should be connected to it simultaneously

            while (Joining.Exchange(true, std::memory_order_acquire));

#ifdef JOINER_VERBOSE_FILE
            Locker().lock();
            Log() << TreeID << " join_less with " << otherTree.TreeID
                      << " This level: " << Level << ", prev_other_level: " << PrevLeftLevel
                      << ", cur other_level: " << otherTree.Level << std::endl;

            Log() << "This height: " << Tree->height() << ", other height: " << otherTree.Tree->height() << std::endl;
            Locker().unlock();
#endif

#ifdef JOINER_CHECK
            size_t treeSize = Tree->size_slow();
            size_t otherTreeSize = otherTree.Tree->size_slow();
#endif

            //CheckLevel(PrevLeftLevel, otherTree.Level);

            bool res;
            TKey key = otherTree.MaxKey;

            // TODO: CHECK
            if (otherTree.Tree->empty())
                return;

            do {
                size_t otherTreeRootLevel = otherTree.Tree->height() - 1;
                res = Tree->join_less(key, GetLeftSpine(), *otherTree.Tree, GetRightSpine(), &key);
                if (!res) {
                    otherTree.Tree->add_root(otherTree.GetLeftSpine());
                    otherTree.Tree->add_root(otherTree.GetRightSpine());
                }

                if (otherTreeRootLevel > 0)
                    GetLeftSpine()[otherTreeRootLevel - 1] = otherTree.GetLeftSpine()[otherTreeRootLevel - 1];
            }
            while (!res);


            Height = Tree->height_danger();

#ifdef JOINER_CHECK
            Tree->verify_node_no_leaf_links();
            if (!Tree->check_spines(GetLeftSpine(), GetRightSpine()))
                throw std::logic_error("The spines are incorrect in tree " + std::to_string(TreeID));

            size_t newTreeSize = Tree->size_slow();
            if (newTreeSize != treeSize + otherTreeSize)
            {
                std::cout << "Tree: " << TreeID << " other tree: " << otherTree.TreeID << std::endl;
                std::cout << "Error: New tree size: " << newTreeSize
                << ", expercted: " << treeSize + otherTreeSize
                << ", tree size: " << treeSize
                << ", other tree size: " << otherTreeSize << std::endl;

                std::terminate();
            }
            else
            {
                Log() << "New tree size: " << newTreeSize
                << ", expercted: " << treeSize + otherTreeSize
                << ", tree size: " << treeSize
                << ", other tree size: " << otherTreeSize << std::endl;
            }
#endif

            Joining = false;
        }

        void JoinGreater(TMetaTree& otherTree)
        {
            // Necessary: consider the situation when we have three trees.
            // Central tree is the highest. Then Its left and right neighbours
            // should be connected to it simultaneously
            while (Joining.Exchange(true, std::memory_order_acquire));


#ifdef JOINER_VERBOSE_FILE
            Locker().lock();
            Log() << TreeID << " join_greater with " << otherTree.TreeID
                      << " This level: " << Level << ", prev_other_level: " << PrevRightLevel
                      << ", cur other_level: " << otherTree.Level << std::endl;

            Log() << "This height: " << Tree->height() << ", other height: " << otherTree.Tree->height() << std::endl;
            Locker().unlock();
#endif

#ifdef JOINER_CHECK
            size_t treeSize = Tree->size_slow();
            size_t otherTreeSize = otherTree.Tree->size_slow();
#endif
            //CheckLevel(PrevRightLevel, otherTree.Level);

            bool res;
            TKey key = MaxKey;

            // TODO: CHECK
            if (otherTree.Tree->empty())
                return;

            do
            {
                size_t otherTreeRootLevel = otherTree.Tree->height() - 1;
                res = Tree->join_greater(key, GetRightSpine(), *otherTree.Tree, GetLeftSpine(), &key);
                if (!res)
                {
                    otherTree.Tree->add_root(otherTree.GetLeftSpine());
                    otherTree.Tree->add_root(otherTree.GetRightSpine());
                }

                // update pointer to node for the case if next join
                // will appear on curlevel - 1
                if (otherTreeRootLevel > 0)
                    GetRightSpine()[otherTreeRootLevel - 1] = otherTree.GetRightSpine()[otherTreeRootLevel - 1];
            }
            while (!res);

            MaxKey = otherTree.MaxKey;
            Height = Tree->height_danger();

#ifdef JOINER_CHECK
            Tree->verify_node_no_leaf_links();
            if (!Tree->check_spines(GetLeftSpine(), GetRightSpine()))
                throw std::logic_error("The spines are incorrect in tree " + std::to_string(TreeID));


            size_t newTreeSize = Tree->size_slow();
            if (newTreeSize != treeSize + otherTreeSize)
            {
                std::cout << "Tree: " << TreeID << " other tree: " << otherTree.TreeID << std::endl;
                std::cout << "Error: New tree size: " << newTreeSize
                << ", expercted: " << treeSize + otherTreeSize
                << ", tree size: " << treeSize
                << ", other tree size: " << otherTreeSize << std::endl;

                std::terminate();
            }
            else
            {
                Log() << "New tree size: " << newTreeSize
                << ", expercted: " << treeSize + otherTreeSize
                << ", tree size: " << treeSize
                << ", other tree size: " << otherTreeSize << std::endl;
            }
#endif

            Joining = false;
        }

    private:

        inline void CheckLevel(uint8_t& prevLvl, uint8_t lvl)
        {
            if (prevLvl > lvl + 1)
            {
                std::cout << "Error, Tree: " << TreeID << std::endl;
                std::cout << "Previous level " << prevLvl
                << " greater than current lvl of tree for join "
                << lvl << std::endl;
                throw std::logic_error("Incorrect lvl");
            }
            prevLvl = lvl;
        }
    };

    static_assert(sizeof(TMetaTree) % 64 == 0, "Strucuture size should be multiple to size of a cache line(64 bytes)");

    typedef typename std::vector<TMetaTree> TTrees;
    typedef typename TTrees::iterator TIterator;


public:

    std::vector<TMetaTree> Trees;
    std::vector<uint8_t> Priority;


    TTreeJoiner(size_t size)
    {
        Trees.reserve(size + 2);
        Init(size);
    }

    void Reset(size_t size)
    {
        Trees.clear();
        Priority.clear();

        Trees.reserve(size + 2);
        Priority.reserve(size + 2);

        Init(size);
    }

    TIterator begin()
    {
        return ++Trees.begin();
    }

    TIterator end()
    {
        return --Trees.end();
    }

    void print()
    {
        for (auto el : Trees)
            std::cout << el.Tree << ' ';
        std::cout << std::endl;
    }

    void AddTree(size_t id, TTree* tree)
    {
        Trees[id + 1].Tree = tree;
    }

    std::vector<TMetaTree>& GetTrees()
    {
        return Trees;
    }

    inline uint8_t GetPrev(uint8_t prev)
    {
        while (!Trees[prev].Active)
            --prev;

        return prev;
    }

    inline uint8_t GetNext(uint8_t next)
    {
        while (!Trees[next].Active)
            ++next;

        return next;
    }

    uint32_t GetNewID(TIterator cur)
    {
        uint32_t num = 0;
        while (cur->Active && cur->Level != TMetaTree::MaxLevel)
        {
            --cur;
            ++num;
        }
        return num;
    }

private:
    void Init(size_t size)
    {
        Trees.emplace_back(nullptr, 0);
        Trees.front().Iteration = TMetaTree::MaxIter;

        for (size_t i = 0; i < size; ++i)
            Trees.emplace_back(nullptr, i + 1);

        Trees.emplace_back(nullptr, Trees.size());
        Trees.back().Iteration = TMetaTree::MaxIter;

        SetPriorities();
    }

    void SetPriorities()
    {
        Priority.push_back(0);

        size_t size = Trees.size() - 2;
        for (size_t i = 0; i < size; ++i)
            Priority.push_back(Trees[i + 1].TreeID);

        Priority.push_back(0);

        std::random_shuffle(Priority.begin() + 1, Priority.end() - 1);
    }
};

template <typename TTree>
constexpr char TTreeJoiner<TTree>::TMetaTree::Undefined;

template <typename TTree>
constexpr uint8_t TTreeJoiner<TTree>::TMetaTree::MaxIter;

template <typename TTree>
constexpr uint8_t TTreeJoiner<TTree>::TMetaTree::MaxLevel;

template <typename TTree>
void StartJoiner(TTreeJoiner<TTree>& joiner, typename TTreeJoiner<TTree>::TIterator cur)
{
    Joiner(joiner, cur);
}

template <typename TTree>
void Joiner(TTreeJoiner<TTree>& joiner, typename TTreeJoiner<TTree>::TIterator cur1)
{
    using TMetaTree = typename TTreeJoiner<TTree>::TMetaTree;

    using TTrees = typename TTreeJoiner<TTree>::TTrees;

    TTrees& trees = joiner.Trees;

    uint8_t cur(cur1 - trees.begin());

    uint8_t next = cur + 1;
    uint8_t prev = cur - 1;


    while (true)
    {

        next = joiner.GetNext(next);
        prev = joiner.GetPrev(prev);

        if (prev == 0 && next + 1 == (uint8_t)trees.size())
            break;

        // wait for actual heights from previous iter
        uint8_t prHt = trees[prev].Height;
        uint8_t nxtHt = trees[next].Height;
        uint8_t ht = trees[cur].Height;

        //std::atomic_thread_fence(std::memory_order_acquire);

        //++cur->ReadLevelPhaseNum;

        // 1. Wait while neighbours will read actual height of current tree
        // - not necessary because we use cur->height
        // and other threads will read actual active status of this tree.
        // 2. Prevent thread skiped tree which became inactive on this
        // iteration. Bad for border case: A B end. B becomes inactive before A calculates its prev and next.
        // A skips B and exits. And B waits for A. Fixed by moving if (!cur->Active) below.
        // 3. Situation: A B C D. h(A) > h(B) > h(C). h(D) > h(B) > h(C).
        // We first join C to B. B sees C is inactive and its neighbours A and D.
        // B joins itself to A but join of C to B is not finished yet.

        //while (cur->ReadLevelPhaseNum > prev->ReadLevelPhaseNum);
        //while (cur->ReadLevelPhaseNum > next->ReadLevelPhaseNum);

        bool active = true;
        bool nextPrior = joiner.Priority[next] < joiner.Priority[cur];

        if ((ht < prHt && ht < nxtHt) || (ht < prHt && ht == nxtHt && nextPrior))
        {
            active = false;
            if (prHt != TMetaTree::MaxLevel)
            {
                //PRINT1(std::to_string(trees[prev].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration));
                //log += std::to_string(trees[prev].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration) + '\t';

                //CLOCK_START
                trees[prev].JoinGreater(*cur1);
                //CLOCK_END("Join")
                //trees[prev].Height = trees[prev].Tree ? trees[prev].Tree->height() : TMetaTree::MaxLevel;
            }
            else
            {
                //PRINT1(std::to_string(trees[next].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration));
                //log += std::to_string(trees[next].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration) + '\t';
                //CLOCK_START
                trees[next].JoinLess(*cur1);
                //CLOCK_END("Join")
                //trees[next].Height = trees[next].Tree ? trees[next].Tree->height() : TMetaTree::MaxLevel;
            }
        }
        else
        {
            if (ht == prHt && ht <= nxtHt)
            {
                bool prevPrior = joiner.Priority[prev] < joiner.Priority[cur];
                //while (prev->Color == TMetaTree::Undefined);
                if ((ht < nxtHt && prevPrior) || (ht == nxtHt && prevPrior && nextPrior))
                {
                    active = false;

                    //PRINT1(std::to_string(trees[prev].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration));
                    //log += std::to_string(trees[prev].TreeID) + " ! " + std::to_string(trees[cur].TreeID) + " ! " + std::to_string(trees[cur].Iteration) + '\t';

                    //CLOCK_START
                    trees[prev].JoinGreater(*cur1);
                    //CLOCK_END("Join")
                    //trees[prev].Height = trees[prev].Tree ? trees[prev].Tree->height() : TMetaTree::MaxLevel;
                }
            }
        }

        // update neighbours' prev and next if inactive
        if (!active)
        {
            //trees[next].Prev = prev;
            //trees[prev].Next = next;
            //PRINT1(std::to_string(prev) + " ! " + std::to_string(cur) + " ! " + std::to_string(next));

            //std::atomic_thread_fence(std::memory_order_acquire);

            trees[cur].Active = false;

            return;
        }
        else
            ++trees[cur].Iteration;

        // 1. Wait for actual Active status of neighbours.
        // 2. Wait until the join will be done by neighbours.
        // This allows to remove cur->Busy usage and
        // cur->Next and cur->Prev will be updated by
        // corresponded neighbours.
        while (trees[cur].Iteration > trees[next].Iteration)
        {
            if (!trees[next].Active)
                ++next;
        }

        while (trees[cur].Iteration > trees[prev].Iteration)
        {
            if (!trees[prev].Active)
                --prev;
        }
    }
    joiner.Trees[1].Tree = trees[cur].Tree;
}
