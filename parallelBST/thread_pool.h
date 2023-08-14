#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

#ifdef __gnu_linux__
#include <pthread.h>
void PinToCore(size_t core)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}
#endif

template<typename T>
class TThreadsafeQueue
{
private:
    struct TNode
    {
        std::shared_ptr<T> Data;
        std::unique_ptr<TNode> Next;
    };

    std::mutex HeadMutex;
    std::unique_ptr<TNode> Head;
    std::mutex TailMutex;
    TNode* Tail;

    TNode* GetTail()
    {
        std::lock_guard<std::mutex> tailLock(TailMutex);
        return Tail;
    }

    bool PopHead(T& value)
    {
        std::lock_guard<std::mutex> headLock(HeadMutex);

        if (Head.get() == GetTail())
        {
            return false;
        }

        value = std::move(*Head->Data);
        std::unique_ptr<TNode> const oldHead = std::move(Head);
        Head = std::move(oldHead->Next);
        return oldHead.get() != nullptr;
    }

    template <typename TType>
    void PushImpl(TType&& newValue)
    {
        std::shared_ptr<T> newData(std::make_shared<T>(std::forward<TType>(newValue)));
        std::unique_ptr<TNode> p(new TNode);
        TNode* const newTail = p.get();
        std::lock_guard<std::mutex> tailLock(TailMutex);
        Tail->Data = newData;
        Tail->Next = std::move(p);
        Tail = newTail;
    }


public:
    TThreadsafeQueue()
            :   Head(new TNode)
            ,   Tail(Head.get())
    {}

    TThreadsafeQueue(const TThreadsafeQueue& other) = delete;
    TThreadsafeQueue& operator= (const TThreadsafeQueue& other) = delete;

    bool TryPop(T& value)
    {
        return PopHead(value);
    }

    void Push(T&& newValue)
    {
        PushImpl(std::move(newValue));
    }

    void Push(const T& newValue)
    {
        PushImpl(newValue);
    }
};




class TFunctionWrapper
{
private:
    struct TBaseImpl
    {
        virtual void Call() = 0;
        virtual ~TBaseImpl() {}
    };

    template<typename TFunctor>
    struct TImplType : TBaseImpl
    {
        TFunctor F;

        TImplType(TFunctor&& f)
                :   F(std::move(f))
        {}

        void Call() override
        {
            F();
        }
    };

    std::unique_ptr<TBaseImpl> ImplPtr;

public:

    template <typename TFunc>
    TFunctionWrapper(TFunc&& f)
            :   ImplPtr(new TImplType<TFunc>(std::forward<TFunc>(f)))
    {}

    void operator() ()
    {
        ImplPtr->Call();
    }

    TFunctionWrapper()
    {}

    ~TFunctionWrapper()
    {}

    TFunctionWrapper(TFunctionWrapper&& functionWrapper)
            :   ImplPtr(std::move(functionWrapper.ImplPtr))
    {}

    TFunctionWrapper& operator=(TFunctionWrapper&& functionWrapper)
    {
        ImplPtr = std::move(functionWrapper.ImplPtr);
        return *this;
    }

    bool Empty() const
    {
        return ImplPtr == nullptr;
    }

private:
    TFunctionWrapper(const TFunctionWrapper&) = delete;
    TFunctionWrapper(TFunctionWrapper&) = delete;
    TFunctionWrapper& operator=(const TFunctionWrapper&) = delete;
    TFunctionWrapper& operator=(TFunctionWrapper&) = delete;
};

class TThreadJoiner
{
private:
    std::vector<std::thread>& Threads;

public:
    TThreadJoiner(std::vector<std::thread>& threads)
            :   Threads(threads)
    {}

    void Clear()
    {
        for (auto& thread : Threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }


    ~TThreadJoiner()
    {
        Clear();
    }
};

class TThreadPool
{
private:
    TThreadsafeQueue<TFunctionWrapper> TaskQueue;
    std::atomic_bool Done;
    std::vector<std::thread> Threads;
    TThreadJoiner ThreadJoiner;

    void Worker(
#ifdef __gnu_linux__
            uint32_t id
#endif
    )
    {
#ifdef __gnu_linux__
        PinToCore(id);
#endif
        while (!Done)
        {
            TFunctionWrapper task;
            if (TaskQueue.TryPop(task))
                task();
            //else
            //    std::this_thread::yield();
        }
    }

public:
    explicit TThreadPool(size_t threadsCount = 0)
            :   ThreadJoiner(Threads)
    {
        Done = false;
        size_t threadCount = threadsCount == 0 ? std::thread::hardware_concurrency() : threadsCount;
        try
        {
            Threads.reserve(threadCount);
            for (size_t i = 0; i < threadCount; ++i)
                Threads.push_back(std::thread(&TThreadPool::Worker, this
#ifdef __gnu_linux__
                        , i + 1
#endif
                ));
        }
        catch (...)
        {
            Done = true;
            throw;
        }
    }

    void Resize(size_t threadsCount)
    {
        Done = true;
        ThreadJoiner.Clear();
        Threads.clear();

        Done = false;
        Threads.reserve(threadsCount);
        for (size_t i = 0; i < threadsCount; ++i)
            Threads.push_back(std::thread(&TThreadPool::Worker, this
#ifdef __gnu_linux__
                    , i + 1
#endif
            ));

    }

    ~TThreadPool()
    {
        Done = true;
    }

    template <typename TFunctor, typename... TArgs>
    std::future<typename std::result_of<TFunctor(TArgs...)>::type> Submit(TFunctor&& f, TArgs... args)
    {
        typedef typename std::result_of<TFunctor(TArgs...)>::type TResultType;
        std::packaged_task<TResultType()> task(std::bind(std::forward<TFunctor>(f), std::forward<TArgs>(args)...));
        std::future<TResultType> res(task.get_future());

        TaskQueue.Push(std::move(task));

        return res;
    }

};

