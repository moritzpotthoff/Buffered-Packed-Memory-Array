#pragma once

#include <atomic>
#include <memory>
#include <type_traits>
#include <vector>
#include <unordered_set>

#include <btree_types.h>
#include <numeric.h>
#include <thread_pool.h>

//#define SPLIT_TEST
//#define INSERT_TEST
//#define JOIN_TEST

template <typename TBtree>
struct TMetaTree
{
    typedef typename TBtree::key_type TKey;

    std::atomic<TBtree*> Tree;
    TKey Key;
    std::atomic_bool InsertPhaseEnd;
    //std::atomic_bool Done;
    volatile bool Done;
    //TBtree* Tree;
    //std::atomic<typename TBtree::key_type> Key;

    uint8_t Dummy[64 - sizeof(Tree) - sizeof(TKey) - 2 * sizeof(std::atomic_bool)];
    //uint8_t Dummy[572 - sizeof(Tree) - sizeof(TKey) - 2 * sizeof(std::atomic_bool)];

    TMetaTree()
            :   Tree(new TBtree)
            ,   Key()
            ,   InsertPhaseEnd(false)
            ,   Done(false)
    {
        static_assert(sizeof(TMetaTree<TBtree>) % 64 == 0,
                      "Strucuture size should be multiple to size of a cache line(64 bytes)");
    }

    void Finish()
    {
        delete Tree.load();

        Tree.store(new TBtree);
        Key = TKey();
        InsertPhaseEnd = false;
        Done = false;
    }

    ~TMetaTree()
    {
        delete Tree.load();
    }
};

// Generates uniform data
// in range [min, max)
template <typename TKeyType>
struct TUniformDataGenerator
{
    static const char* Name() {
        return "Uniform";
    }
    void operator() (std::vector<TKeyType>& data, size_t size, size_t min, size_t max, uint32_t seed = 0)
    {
        TThreadRandom<TKeyType> rnd(seed);

        for (size_t i = 0; i < size; ++i)
        {
            TKeyType key = rnd(min, max);
            data.push_back(key);
        }

        std::sort(data.begin(), data.end());
//        auto end = std::unique(data.begin(), data.end());
//        data.resize(end - data.begin());
    }
};

// Generates uniform data
// in range [seed * min, seed + max)
template <typename TKeyType>
struct TUniformIncreasingDataGenerator
{
    static const char* Name() {
        return "UniformIncreasing";
    }
    void operator() (std::vector<TKeyType>& data, size_t size, size_t min, size_t max, uint32_t seed = 0)
    {
        TThreadRandom<TKeyType> rnd(seed);

        size_t range = size;
        min = max + seed * range;
        max = min + range;

        for (size_t i = 0; i < size; ++i)
        {
            TKeyType key = rnd(min, max);
            data.push_back(key);
        }

        std::sort(data.begin(), data.end());
//        auto end = std::unique(data.begin(), data.end());
//        data.resize(end - data.begin());
    }
};

template <typename TKeyType>
struct TNormalDistributionDataGenerator
{
    static const char* Name() {
        return "Normal";
    }
    void operator() (std::vector<TKeyType>& data, size_t size, size_t min, size_t max, uint32_t seed = 0)
    {
        std::mt19937 gen((TKey(seed)));
        std::normal_distribution<> d(min + (max - min) / 2, min + (max - min) / 262144);

        for (size_t i = 0; i < size; ++i)
        {
            data.push_back(min + std::lround(d(gen)) % (max - min + 1));
        }

        std::sort(data.begin(), data.end());
//        auto end = std::unique(data.begin(), data.end());
//        data.resize(end - data.begin());
    }
};

template <typename TKeyType>
struct TUniformRangeDataGenerator
{
    static const char* Name() {
        return "UniformRange";
    }

    static TThreadRandom<TKeyType> Rnd;

    void operator() (std::vector<TKeyType>& data, size_t size, size_t min, size_t max, uint32_t seed = 0)
    {
        if (size > max - min) {
            throw std::logic_error("Size should be less than range");
        }

        // we want to generate keys in a subrange of [min, max) of the size (3 * size)
        size_t offset = Rnd(0, max - 3 * size + 1);

        TThreadRandom<TKeyType> rnd(seed);

        //newData.push_back(min);
        for (size_t i = 0; i < size; ++i)
        {
            TKeyType key = rnd(offset, offset + 3 * size);
            data.push_back(key);
        }

        std::sort(data.begin(), data.end());
//        auto end = std::unique(data.begin(), data.end());
//        data.resize(end - data.begin());
    }
};

template <typename TKeyType>
TThreadRandom<TKeyType> TUniformRangeDataGenerator<TKeyType>::Rnd(0);

template<typename TBtree, bool TParallelJoiner = false>
struct TDataInserter
{
    void operator() (TMetaTree<TBtree>& tree, std::vector<typename TBtree::key_type>& data, size_t tasksNumber, size_t id)
    {
        TBtree* treePtr = tree.Tree;

        // There is a PROBLEM!!!
        // It's possible that during previous joins
        // there are a duplicates in leaves. Using std::unique
        // in insert_sorted can cause that some nodes have less
        // than minimum possible number of keys.

        size_t size = data.size() / tasksNumber;
        auto begin = data.begin() + size * id;
        size_t offset = id != tasksNumber - 1 ? size : data.end() - begin;

        if (!treePtr->empty())
            treePtr->insert_sorted(begin, begin + offset);
        else
            treePtr->bulk_load(begin, begin + offset);

        if (!TParallelJoiner)
            tree.Key = treePtr->max_key_slow();

        if (id & 1)
            tree.Done = true;

        tree.InsertPhaseEnd = true;
    }
};

template<typename TBtree, bool TParallelJoiner = false>
struct TSlowDataInserter
{
    void operator() (TMetaTree<TBtree>& tree, std::vector<typename TBtree::key_type>& data, size_t tasksNumber, size_t id)
    {
        TBtree* treePtr = tree.Tree;
        size_t size = data.size() / tasksNumber;
        auto begin = data.begin() + size * id;
        size_t offset = id != tasksNumber - 1 ? size : data.end() - begin;

        if (!treePtr->empty())
            for (auto it = begin; it != begin + offset; ++it)
                treePtr->insert(*it);
        else
            treePtr->bulk_load(begin, begin + offset);

        if (!TParallelJoiner)
            tree.Key = treePtr->max_key_slow();

        if (id & 1)
            tree.Done = true;

        tree.InsertPhaseEnd = true;
    }
};

template <typename TBtree>
struct TParallelJoin;

template <
        typename TBtree,
        template <typename, bool> class TDataInserter,
        typename TSpliter,
        typename TJoiner
#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
        , typename TTimer
#endif
        >
class TBulkInserter
{
private:
    using TInserter = TDataInserter<TBtree, std::is_same<TJoiner, TParallelJoin<TBtree>>::value>;


    using TKeyType = typename TBtree::key_type;
    using TPairType = typename TBtree::pair_type;
    using TMainClass = TBulkInserter<TBtree, TDataInserter, TSpliter, TJoiner
#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
            , TTimer
#endif
            >;

    std::atomic_bool Done;
    std::atomic_uint_fast8_t Counter;

#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
    std::atomic_uint_fast8_t CounterSplit;
    std::atomic_uint_fast8_t CounterInsert;
#endif

    std::atomic_uint_fast16_t WorkersNumber;

    uint8_t PoolThreadsNumber;
    TThreadPool pool;
    std::atomic_uint_fast32_t StartFlag;

    TBtree* TreePtr;
    std::vector<TKeyType>* DataPtr;
    std::unique_ptr<TMetaTree<TBtree>[]> Trees;

#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
    TTimer* TimerPtr;
#endif

public:
    explicit TBulkInserter(uint8_t threadNumber = 0)
        :   Done(false)
        ,   Counter(0)
#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
        ,   CounterSplit(0)
        ,   CounterInsert(0)
#endif
        ,   WorkersNumber(0)
        ,   PoolThreadsNumber(threadNumber - 1)
        ,   pool(std::max(PoolThreadsNumber, static_cast<uint8_t>(1)))
        ,   StartFlag(0)
        ,   TreePtr(nullptr)
        ,   DataPtr(nullptr)
        ,   Trees(new TMetaTree<TBtree>[threadNumber])
#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
        ,   TimerPtr(nullptr)
#endif
    {
        for (uint8_t i = 0; i < PoolThreadsNumber; ++i)
            pool.Submit(TWorker(*this, i + 1));

        TJoiner::Init(threadNumber);
        while (WorkersNumber != PoolThreadsNumber);
    }

    ~TBulkInserter()
    {
        Done = true;
    }


#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
    void operator() (TBtree& tree, std::vector<TKeyType>& data, TTimer& timer)
#else
    void operator() (TBtree& tree, std::vector<TKeyType>& data)
#endif
    {
#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
        TimerPtr = &timer;
#endif

        Reset();
        TreePtr = &tree;
        DataPtr = &data;

        // Start execution
        ++StartFlag;

        Start(0);

        while (Counter != PoolThreadsNumber);
        Trees.reset(new TMetaTree<TBtree>[PoolThreadsNumber + 1]);
    }

protected:
    void Reset()
    {
        TSpliter::Reset();
        TJoiner::Reset();
        Counter = 0;

#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
        CounterSplit = 0;
        CounterInsert = 0;
#endif
    }

    struct TWorker
    {
        TMainClass& Host;
        uint32_t Counter;
        uint32_t Id;

        TWorker(TMainClass& host, uint32_t id)
            :   Host(host)
            ,   Counter(0)
            ,   Id(id)
        {}

        void operator() ()
        {
            ++Host.WorkersNumber;
            while (!Host.Done)
            {
                while (Host.StartFlag <= Counter)
                {
                    if (Host.Done)
                        return;
                }
                Host.Start(Id);
                ++Counter;
                ++Host.Counter;
            }
        }
    };


    void Start(uint8_t id)
    {
        TBtree& tree = *TreePtr;
        std::vector<TKeyType>& data = *DataPtr;
        uint8_t tasksNumber = PoolThreadsNumber + 1;

        TSpliter spliter(tree, Trees.get(), id, data, tasksNumber);
        TInserter inserter;
        TJoiner joiner;
        joiner.AddTree(id, Trees[id].Tree.load());

        // Split tree
#ifdef SPLIT_TEST
        ++CounterSplit;
        while (CounterSplit != tasksNumber);

        TimerPtr->Submit("Split", std::ref(spliter));
#else
        spliter();
#endif

        // Insert data
#ifdef INSERT_TEST
        ++CounterInsert;
        while (CounterInsert != tasksNumber);

        TimerPtr->Submit("Insert", std::ref(inserter), std::ref(Trees[id]), std::ref(data), tasksNumber, id);
#else
        inserter(Trees[id], data, tasksNumber, id);
#endif
        // Join trees
        BTREE_VISIT_NODE_RESET

        joiner.InitTreeAndWait(id);

#ifdef JOIN_TEST
        TimerPtr->Submit("Join", std::ref(joiner), std::ref(tree), Trees.get(), id, tasksNumber);
#else
        joiner(tree, Trees.get(), id, tasksNumber);
#endif

        joiner.GetResult(id, &tree);

        //Trees[id].Finish();
    }
};

#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
template <typename TBtree, typename TTimer>
#else
template <typename TBtree>
#endif
class TBtreeSequentialBulkInsert
{
public:
    using TTree = TBtree;
    using TKeyType = typename TBtree::key_type;

    explicit TBtreeSequentialBulkInsert(uint8_t)
    {}

    static void CopyToVector(TTree& tree, std::vector<TKeyType>& data)
    {
        tree.to_vector(data);

        auto end = std::unique(data.begin(), data.end());
        data.resize(end - data.begin());
    }


#if defined(SPLIT_TEST) || defined(INSERT_TEST) || defined(JOIN_TEST)
    void operator() (TBtree& tree, std::vector<TKeyType>& data, TTimer&)
#else
    void operator() (TBtree& tree, std::vector<TKeyType>& data)
#endif
    {
        if (!tree.empty())
            tree.insert_sorted(data.begin(), data.end());
        else
            tree.bulk_load(data.begin(), data.end());
    }
};

template <typename _TTree>
class TSTLUnion
{
public:
    using TTree = _TTree;
    using TKeyType = typename TTree::key_type;

    static void CopyToVector(TTree& tree, std::vector<TKeyType>& data)
    {
        for (const auto& it : tree)
            data.push_back(it);

        auto end = std::unique(data.begin(), data.end());
        data.resize(end - data.begin());
    }

    explicit TSTLUnion(uint8_t)
    {}

    void operator() (TTree& tree, const std::vector<TKeyType>& data)
    {
        TTree res;
        std::set_union(tree.begin(), tree.end(), data.begin(), data.end(), std::inserter(res, res.end()));
        tree.swap(res);
    }
};