#ifndef ZMQEXECUTORLIB_TASKQUEUE_H
#define ZMQEXECUTORLIB_TASKQUEUE_H

#include <boost/lockfree/queue.hpp>
#include <iostream>

/**
 * Thread safe Queue to store and automatically execute tasks on retrieve.
 */
class TaskQueue {
private:
    boost::lockfree::queue<std::function<void()> *> queue = boost::lockfree::queue<std::function<void()> *>(0);
public:
    /**
     * Add a task to the queue. Thread safe.
     */
    void add(std::unique_ptr<std::function<void()>> task) {
        queue.push(task.release());
    }

    /**
     * Retrieves the first task and executes it. Thread safe.
     */
    void execute() {
        std::function<void()> *task;
        queue.pop(task);
        task->operator()();
        delete task;
    }

    ~TaskQueue() {
        std::function<void()> *pointer;
        while (queue.pop(pointer)) {
            delete pointer;
        }
    }
};


#endif //ZMQEXECUTORLIB_TASKQUEUE_H
