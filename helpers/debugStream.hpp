#pragma once
#include <chrono>
#include <algorithm>
#include <vector>
#include <iomanip>

#include "timer.hpp"

namespace Helpers {
    template<typename DataType>
    class DebugInfo {
    public:
        DebugInfo(size_t t, DataType v, std::string m) : time(t), value(v), message(m) {}

        friend bool operator<(const DebugInfo& l, const DebugInfo& r) {
            return std::tie(l.time, l.value, l.message) < std::tie(r.time, r.value, r.message);
        }

        friend std::ostream& operator<<(std::ostream& out, const DebugInfo& info) {
            out << std::setw(12) << info.time << "ns, " << std::setw(12) << info.value << ": " << info.message;
            return out;
        }

        size_t time;
        DataType value;
        std::string message;
    };

    template<typename DataType>
    class NoDebugStream {
    public:
        inline void registerThread() const {}
        inline void add(DataType value, std::string message) const { (void)value; (void)message; }
        inline void add(std::string message) const { (void)message; }
        inline void reserve(size_t numberOfMessages) const { (void)numberOfMessages; }
        inline void print() const {}
        static inline std::string identifier() {
            return "NoDebugStream";
        }
    };

    template<typename DataType>
    class DebugStream {
    public:
        inline void registerThread() {
            auto l = std::scoped_lock(m);
            std::cout << "Registering thread " << std::this_thread::get_id() << std::endl;
            messages[std::this_thread::get_id()];//Create a buffer for this thread's key
        }

        inline void add(DataType value, std::string message) {
            DebugInfo msg(timer.getNanoseconds(), value, message);
            messages.at(std::this_thread::get_id()).push_back(msg);
        }

        inline void add(std::string message) {
            DebugInfo msg(timer.getNanoseconds(), (DataType)std::this_thread::get_id(), message);
            messages.at(std::this_thread::get_id()).push_back(msg);
        }

        inline void reserve(size_t numberOfMessages) {
            for (auto &vec : messages) vec.second.reserve(numberOfMessages);
        }

        inline void print() {
            std::vector<DebugInfo<DataType>> allMessages;
            //merge the thread-local messages
            for (auto &threadMessages : messages) {
                allMessages.insert(allMessages.end(), threadMessages.second.begin(), threadMessages.second.end());
            }
            std::sort(allMessages.begin(), allMessages.end());
            for (auto &info : allMessages) {
                std::cerr << info << std::endl;
            }
        }

        static inline std::string identifier() {
            return "DebugStream";
        }

    private:
        std::map<std::thread::id, std::vector<DebugInfo<DataType>>> messages;
        std::mutex m;
        Timer timer;
    };
}