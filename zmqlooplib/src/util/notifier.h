#ifndef ZMQLOOPLIB_NOTIFIER_H
#define ZMQLOOPLIB_NOTIFIER_H

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

#endif //ZMQLOOPLIB_NOTIFIER_H
