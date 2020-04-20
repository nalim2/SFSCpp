#ifndef ZMQEXECUTORLIB_TASKQUEUE_H
#define ZMQEXECUTORLIB_TASKQUEUE_H

#include <boost/lockfree/queue.hpp>
#include <iostream>

/**
 * Thread safe Queue to store and automatically execute tasks on retreive.
 */
class TaskQueue {
private:
    boost::lockfree::queue<std::function<void()> *> queue = boost::lockfree::queue<std::function<void()> *>(0);
    /**
     * Add a task to the queue. Thread safe. The referenced task will be deleted after popped.
     */
public:
    void add(std::function<void()> *task) {
        queue.push(task);
    }

    /**
     * Retreives the first task, executes it and then deletes it. Thread Safe.
     */
    void execute() {
        std::function<void()> *task;
        queue.pop(task);
        (*task)();
        delete task;
    }
};


#endif //ZMQEXECUTORLIB_TASKQUEUE_H
