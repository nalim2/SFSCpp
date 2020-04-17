#ifndef ZMQEXECUTORLIB_NOTIFIER_H
#define ZMQEXECUTORLIB_NOTIFIER_H

#include <mutex>
#include <condition_variable>
#include <atomic>

class Notifier {
private:
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic_int counter = 0;
    std::atomic_bool stopRequested = false;
public:
    void notify();

    void wait();

    void close();
};

class NotifierClosedException : std::exception {

};

#endif //ZMQEXECUTORLIB_NOTIFIER_H
