#pragma once

#include "barrier.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace threading {

using clock_type = std::chrono::steady_clock;

namespace detail {

struct SharedData {
    unsigned int num_threads;
    Barrier barrier;
    clock_type::time_point timestamp;
    std::unordered_map<std::string, clock_type::duration> times;
    alignas(64) std::atomic_size_t index{0};

    explicit SharedData(unsigned int n) : num_threads{n}, barrier{n} {}
};

}  // namespace detail

class Context {
    template <typename Task>
    friend class TaskHandle;

    detail::SharedData& shared_data_;
    unsigned int id_;

    Context(detail::SharedData& sd, unsigned int id)
        : shared_data_{sd}, id_{id} {}

   public:
    [[nodiscard]] unsigned int get_num_threads() const noexcept {
        return shared_data_.num_threads;
    };

    [[nodiscard]] unsigned int get_id() const noexcept { return id_; }

    void pin_to_core(std::size_t core) {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(core, &set);
        if (int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &set)) {
            throw std::system_error{rc, std::system_category(), "Pin to core failed: "};
        }
    }

    void synchronize() const { shared_data_.barrier.wait(); }

    template <typename CompletionFunc>
    void synchronize(CompletionFunc&& f) const {
        shared_data_.barrier.wait(std::forward<CompletionFunc>(f));
    }

    template <typename Work, typename... Args>
    void execute_synchronized(Work work, Args&&... args) const {
        shared_data_.barrier.wait();
        work(std::forward<Args>(args)...);
        shared_data_.barrier.wait();
    }

    template <typename Work, typename... Args>
    void execute_synchronized_timed(std::string const& name, Work work,
                                    Args&&... args) const {
        shared_data_.barrier.wait(
            [this] { shared_data_.timestamp = clock_type::now(); });
        work(std::forward<Args>(args)...);
        shared_data_.barrier.wait([this, &name] {
            auto duration = clock_type::now() - shared_data_.timestamp;
            shared_data_.times[name] = duration;
        });
    }

    template <typename Iter, typename Work>
    void execute_synchronized_blockwise(Iter begin, Iter end, Work work) const {
        using difference_type =
            typename std::iterator_traits<Iter>::difference_type;
        static constexpr difference_type block_size = 1 << 12;

        difference_type n = end - begin;
        shared_data_.barrier.wait();
        while (true) {
            auto block_begin =
                static_cast<difference_type>(shared_data_.index.fetch_add(
                    block_size, std::memory_order_relaxed));
            if (block_begin >= n) {
                break;
            }
            auto block_end = std::min(block_begin + block_size, n);
            work(begin + block_begin, begin + block_end);
        }
        shared_data_.barrier.wait(
            [this] { shared_data_.index.store(0, std::memory_order_relaxed); });
    }

    template <typename Iter, typename Work>
    void execute_synchronized_blockwise_timed(std::string const& name, Iter begin,
                                              Iter end, Work work) const {
        using difference_type =
            typename std::iterator_traits<Iter>::difference_type;
        static constexpr difference_type block_size = 1 << 12;

        difference_type n = end - begin;
        shared_data_.barrier.wait(
            [this] { shared_data_.timestamp = clock_type::now(); });
        while (true) {
            auto block_begin =
                static_cast<difference_type>(shared_data_.index.fetch_add(
                    block_size, std::memory_order_relaxed));
            if (block_begin >= n) {
                break;
            }
            auto block_end = std::min(block_begin + block_size, n);
            work(begin + block_begin, begin + block_end);
        }
        shared_data_.barrier.wait([this, &name] {
            auto duration = clock_type::now() - shared_data_.timestamp;
            shared_data_.times[name] = duration;
            shared_data_.index.store(0, std::memory_order_relaxed);
        });
    }
};

template <typename Task>
class TaskHandle {
    std::vector<std::thread> threads_;
    detail::SharedData shared_data_;

   public:
    template <typename... Args>
    explicit TaskHandle(unsigned int num_threads, Args... args)
        : threads_(num_threads), shared_data_(num_threads) {
        for (unsigned int i = 0; i < threads_.size(); ++i) {
            Context ctx{shared_data_, i};
            threads_[i] = std::thread(Task::run, ctx, args...);
        }
    }

    TaskHandle(TaskHandle const&) = delete;
    TaskHandle(TaskHandle&&) noexcept = default;
    TaskHandle& operator=(TaskHandle const&) = delete;
    TaskHandle& operator=(TaskHandle&&) noexcept = default;
    ~TaskHandle() = default;

    void join() {
        for (auto& t : threads_) {
            t.join();
        }
    }

    auto get_duration(std::string const& name) {
        return shared_data_.times.at(name);
    }
};

}  // namespace threading
