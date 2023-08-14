#pragma once

#include <memory.h>

template<typename T, typename TAllocator>
class TFastList;

template<typename T, typename TAllocator = TMainAllocator<T> >
class TFastListIterator {
private:
    typedef typename TFastList<T, TAllocator>::TChunk TChunk;

    TChunk* Chunk;

public:
    explicit TFastListIterator(TChunk* chunk = 0)
            :   Chunk(chunk)
    {
    }

    TFastListIterator& operator++() {
        Chunk = Chunk->Next;
        return *this;
    }

    T& operator* () const {
        return Chunk->Value;
    }

    T* operator-> () const {
        return &Chunk->Value;
    }

    bool operator== (const TFastListIterator& iter) const {
        return Chunk == iter.Chunk;
    }

    bool operator!= (const TFastListIterator& iter) const {
        return !(*this == iter);
    }

    friend class TFastList<T, TAllocator>;
};

template<typename T, typename TAllocator = TMainAllocator<T> >
class TFastList {
public:
    struct TChunk {
        T Value;
        TChunk* Next;
        TChunk* Previous;

        explicit TChunk(const T& value, TChunk* next = 0, TChunk* previous = 0)
                :   Value(value)
                    ,   Next(next)
                    ,   Previous(previous)
        {}
    };

    typedef TFastListIterator<T, TAllocator> TIterator;
    typedef TFastListIterator<T, TAllocator> iterator;

private:
    typedef T TType;
    typedef typename TAllocator::template TRebinder<TChunk>::TOther TChunkAllocator;

    TChunk* BeginChunk;
    TChunk* EndChunk;

    uint32_t Size;

    TChunkAllocator Allocator;

public:
    TFastList()
            :   BeginChunk(0)
                ,   EndChunk(0)
                ,   Size(0)
    {}

    ~TFastList() {
        while (!empty()) {
            pop_front();
        }
    }

    bool empty() const {
        return Size == 0;
    }

    void push_front(const T& value) {
        TChunk* newElem = Allocator.allocate();
        Allocator.construct(newElem, TChunk(value));

        newElem->Next = BeginChunk;

        if (BeginChunk) {
            BeginChunk->Previous = newElem;
        }

        BeginChunk = newElem;

        if (Size == 0) {
            EndChunk = BeginChunk;
        }

        ++Size;
    }

    void pop_front() {
        if (empty()) {
            throw std::logic_error("List is empty");
        }

        TChunk* prevBegin = BeginChunk;
        BeginChunk = BeginChunk->Next;

        if (BeginChunk) {
            BeginChunk->Previous = 0;
        }

        RemoveNode(prevBegin);
        --Size;
    }

    void erase(iterator iter) {
        TChunk* Next = iter.Chunk->Next;
        TChunk* Previous = iter.Chunk->Previous;

        if (Next) {
            Next->Previous = Previous;
        }

        if (Previous) {
            Previous->Next = Next;
        }

        RemoveNode(iter.Chunk);
        --Size;

        if (iter.Chunk == BeginChunk) {
            BeginChunk = Next;
        }

        if (iter.Chunk == EndChunk) {
            EndChunk = Previous;
        }
    }

    T& front() const {
        return BeginChunk->Value;
    }

    iterator begin() {
        return iterator(BeginChunk);
    }

    iterator end() {
        return iterator(0);
    }

private:
    void RemoveNode(TChunk* chunk) {
        Allocator.destroy(chunk);
        Allocator.deallocate(chunk);
    }
};

template<typename T>
class TFastListWithMemoryPool;

template<typename T>
class TFastListWithMemoryPoolIterator {
private:
    typedef typename TFastListWithMemoryPool<T>::TChunk TChunk;

    TChunk* Chunk;

public:
    explicit TFastListWithMemoryPoolIterator(TChunk* chunk = 0)
            :   Chunk(chunk)
    {
    }

    TFastListWithMemoryPoolIterator& operator++() {
        Chunk = Chunk->Next;
        return *this;
    }

    TFastListWithMemoryPoolIterator& operator--() {
        Chunk = Chunk->Previous;
        return *this;
    }

    TFastListWithMemoryPoolIterator next()
    {
        return TFastListWithMemoryPoolIterator(Chunk->Next);
    }

    TFastListWithMemoryPoolIterator prev()
    {
        return TFastListWithMemoryPoolIterator(Chunk->Previous);
    }

    T& operator* () const {
        return Chunk->Value;
    }

    T* operator-> () const {
        return &Chunk->Value;
    }

    bool operator== (const TFastListWithMemoryPoolIterator& iter) const {
        return Chunk == iter.Chunk;
    }

    bool operator!= (const TFastListWithMemoryPoolIterator& iter) const {
        return !(*this == iter);
    }

    friend class TFastListWithMemoryPool<T>;
};

template<typename T>
class TFastListWithMemoryPool {
public:
    struct TChunk {
        T Value;
        TChunk* Next;
        TChunk* Previous;

        explicit TChunk()
            :   Next(nullptr)
            ,   Previous(nullptr)
        {}

        explicit TChunk(const T& value, TChunk* next = nullptr, TChunk* previous = nullptr)
                    :   Value(value)
                    ,   Next(next)
                    ,   Previous(previous)
        {}
    };

    typedef TFastListWithMemoryPoolIterator<T> TIterator;
    typedef TFastListWithMemoryPoolIterator<T> iterator;
    typedef  TMemoryPool<TChunk> TChunkAllocator;

private:
    typedef T TType;

    TChunk* BeginChunk;
    TChunk* EndChunk;

    uint32_t Size;

    TChunkAllocator* Allocator;

public:
    TFastListWithMemoryPool(TChunkAllocator* allocator = 0)
                :   BeginChunk(0)
                ,   EndChunk(allocator->allocate())
                ,   Size(0)
                ,   Allocator(allocator)
    {
        ::new (EndChunk) TChunk();
        BeginChunk = EndChunk;
    }

    ~TFastListWithMemoryPool() {
        clear();
        RemoveNode(EndChunk);
    }

    bool empty() const {
        //return Size == 0;
        return BeginChunk == EndChunk;
    }

    void push_front(const T& value) {
        insert(iterator(BeginChunk), value);
    }

    void push_back(const T& value)
    {
        insert(iterator(EndChunk), value);
    }

    void pop_front() {
        if (empty()) {
            throw std::logic_error("List is empty");
        }

        erase(iterator(BeginChunk));
    }

    void pop_back()
    {
        if (empty())
            throw std::logic_error("List is empty");

        erase(iterator(EndChunk->Previous));
    }

    void insert(iterator iter, const T& value)
    {
        TChunk* newElem = Allocator->allocate();
        Allocator->construct(newElem, TChunk(value));

        insert(iter, newElem);
    }


    void erase(iterator iter) {
        TChunk* Next = iter.Chunk->Next;
        TChunk* Previous = iter.Chunk->Previous;

        Next->Previous = Previous;

        if (Previous) {
            Previous->Next = Next;
        }

        if (iter.Chunk == BeginChunk) {
            BeginChunk = Next;
        }

        // Will deallocate memory in the end
        //RemoveNode(iter.Chunk);
        //--Size;
    }

    T& front() {
        return BeginChunk->Value;
    }

    T& back()
    {
        return EndChunk->Previous->Value;
    }

    iterator begin() {
        return iterator(BeginChunk);
    }

    iterator end() {
        return iterator(EndChunk);
    }

    void clear()
    {
        while (!empty())
            erase(iterator(BeginChunk));
    }

//    size_t size() const
//    {
//        return Size;
//    }

private:
    void RemoveNode(TChunk* chunk) {
        Allocator->destroy(chunk);
        Allocator->deallocate(chunk);
    }

    void insert(iterator iter, TChunk* newElem)
    {
        TChunk* Previous = iter.Chunk->Previous;

        iter.Chunk->Previous = newElem;

        if (Previous)
            Previous->Next = newElem;

        newElem->Next = iter.Chunk;
        newElem->Previous = Previous;

        if (iter.Chunk == BeginChunk)
            BeginChunk = newElem;

        //++Size;
    }
};
