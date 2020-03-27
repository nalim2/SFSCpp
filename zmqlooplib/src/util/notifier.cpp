#include "notifier.h"

void Notifier::notify() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        counter++;
    }
    condition.notify_all();
}

void Notifier::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [&] { return counter > 0 || stopRequested; });
    if (!stopRequested) {
        counter--;
    } else throw NotifierClosedException();
}

void Notifier::close() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        stopRequested = true;
    }
    condition.notify_all();
}


