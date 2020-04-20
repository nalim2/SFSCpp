#ifndef ZMQEXECUTORLIB_NOTIFIER_H
#define ZMQEXECUTORLIB_NOTIFIER_H

#include <mutex>
#include <condition_variable>
#include <atomic>

/**
 * Exception thrown on wait() call as soon as close() is called.
 */
class NotifierClosedException : std::exception {
};

/**
 * Class used to notify a single producer thread from multiple producer threads.
 * Blocks on wait() until notify() is called.
 */
class Notifier {
private:
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic_int counter = 0;
    std::atomic_bool stopRequested = false;
public:
    /**
     * Notifies a waiter. If there is none, the notification will be reserved and a future wait call will then instantly
     * return. Thread Safe.
     */
    void notify() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            counter++;
        }
        condition.notify_all();
    }

    /**
     * Waits until notified, then decreases counter. Also returns when close is called, in this case
     * NotifierClosedException is thrown. NOT THREAD SAFE! Except if you use only close(), then its okay to use wait()
     * in multiple threads.
     */
    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return counter > 0 || stopRequested; });
        if (!stopRequested) {
            counter--;
        } else throw NotifierClosedException();
    }

    /**
     * Closes this Notifier. Every current and forthcoming wait() call will throw a NotifierClosedException.
     * Thread Safe.
     */
    void close() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stopRequested = true;
        }
        condition.notify_all();
    }
};


#endif //ZMQEXECUTORLIB_NOTIFIER_H
