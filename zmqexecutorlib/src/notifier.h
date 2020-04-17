#ifndef ZMQEXECUTORLIB_NOTIFIER_H
#define ZMQEXECUTORLIB_NOTIFIER_H

#include <mutex>
#include <condition_variable>
#include <atomic>

class NotifierClosedException : std::exception {
};

class Notifier {
private:
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic_int counter = 0;
    std::atomic_bool stopRequested = false;
public:
    void notify() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            counter++;
        }
        condition.notify_all();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return counter > 0 || stopRequested; });
        if (!stopRequested) {
            counter--;
        } else throw NotifierClosedException();
    }

    void close() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stopRequested = true;
        }
        condition.notify_all();
    }
};


#endif //ZMQEXECUTORLIB_NOTIFIER_H
