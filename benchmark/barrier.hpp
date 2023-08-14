#pragma once

#include <pthread.h>

#include <cassert>
#include <exception>
#include <iostream>
#include <system_error>

namespace threading {

class Barrier {
    pthread_barrier_t b_{};

   public:
    explicit Barrier(unsigned int num_threads) {
        assert(num_threads > 0);
        if (int rc = pthread_barrier_init(&b_, nullptr, num_threads); rc != 0) {
            throw std::system_error{rc, std::system_category(),
                                    "Failed to create barrier: "};
        }
    }

    Barrier(Barrier const &) = delete;
    Barrier &operator=(Barrier const &) = delete;
    Barrier(Barrier &&) = delete;
    Barrier &operator=(Barrier &&) = delete;

    void wait() {
        if (int rc = pthread_barrier_wait(&b_);
            rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
            throw std::system_error{rc, std::system_category(),
                                    "Failed to wait for barrier: "};
        }
    }

    template <typename CompletionFunc>
    void wait(CompletionFunc f) {
        if (int rc = pthread_barrier_wait(&b_);
            rc == PTHREAD_BARRIER_SERIAL_THREAD) {
            f();
        } else if (rc != 0) {
            throw std::system_error{rc, std::system_category(),
                                    "Failed to wait for barrier: "};
        }
    }

    ~Barrier() noexcept {
        if (int rc = pthread_barrier_destroy(&b_); rc != 0) {
            std::cerr << "Failed to destroy barrier: "
                      << std::system_category().message(rc) << " (Error " << rc
                      << ")\n";
            std::terminate();
        }
    }
};

}  // namespace threading
