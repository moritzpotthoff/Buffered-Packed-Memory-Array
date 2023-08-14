#pragma once

#include <iostream>

namespace Helpers {
    class Progress {
    public:
        Progress(size_t m) : n(m),
            progress(0),
            percentDone(0) {
            std::cout << "[" << std::flush;
        }

        void operator++() {
            ++progress;
            draw();
        }

        void operator+=(size_t steps) {
            progress += steps;
            draw();
        }

        void done() {
            std::cout << "]" << std::endl << std::flush;
        }

        void reset(size_t newSteps) {
            n = newSteps;
            progress = 0;
            percentDone = 0;

            std::cout << "[" << std::flush;
        }

    private:
        void draw() {
            size_t newDone = progress * 100 / n;//new percentage that is done
            for (size_t i = percentDone; i < newDone; ++i) {
                std::cout << "." << std::flush;
            }
            percentDone = newDone;
        }

    private:
        size_t n;
        size_t progress;
        size_t percentDone;
    };
}