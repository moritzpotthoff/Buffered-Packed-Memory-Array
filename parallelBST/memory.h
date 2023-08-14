#pragma once

#include <exception>
#include <vector>

using std::exception;
using std::vector;

template <typename T>
class TMainAllocator {
public:
    template <typename U>
    struct TRebinder {
        typedef TMainAllocator<U> TOther;
    };

    inline T* allocate() const {
        return static_cast<T*>(::operator new(sizeof(T)));
    }

    inline void deallocate(T* ptr) const {
        ::operator delete(ptr);
    }

    inline void construct(T* ptr, const T& val) const {
        ::new (ptr) T(val);
    }

    inline void destroy(T* ptr) const {
        ptr->~T();
    }
};

template<typename T>
class TMemoryPool {
public:
    template <typename U>
    struct TRebinder {
        typedef TMemoryPool<U> TOther;
    };

    inline T* allocate() {
        if (FreeChunks.empty()) {
            throw std::logic_error("Memory pool is full");
        }

        uint32_t freeChunk = FreeChunks.back();
        FreeChunks.pop_back();
        return static_cast<T*>(static_cast<T*>(Pool) + freeChunk);
    }

    inline void deallocate(T* ptr) {
        FreeChunks.push_back(ptr - static_cast<T*>(Pool));
    }

    inline void construct(T* ptr, const T& val) const {
        ::new (ptr) T(val);
    }

    inline void destroy(T* ptr) const {
        ptr->~T();
    }

    TMemoryPool(uint32_t size)
            :   Pool(::operator new(size * sizeof(T)))
                ,   Size(size)
    {
        UpdateFreeChunks();
    }

    ~TMemoryPool() {
        ::operator delete(Pool);
    }

private:
    void* Pool;
    uint32_t Size;
    vector<size_t> FreeChunks;

    void UpdateFreeChunks() {
        FreeChunks.reserve(Size);

        for (uint32_t i = 0; i < Size; ++i) {
            FreeChunks.push_back(Size - i - 1);
        }
    }
};