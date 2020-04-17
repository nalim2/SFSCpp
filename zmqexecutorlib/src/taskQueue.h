#ifndef ZMQEXECUTORLIB_TASKQUEUE_H
#define ZMQEXECUTORLIB_TASKQUEUE_H

#include <boost/lockfree/queue.hpp>
#include <iostream>

class TaskQueue {
private:
    boost::lockfree::queue<std::function<void()> *> queue = boost::lockfree::queue<std::function<void()> *>(0);
public:
    void add(std::function<void()> *task) {
        queue.push(task);
    }

    void execute() {
        std::function<void()> *task;
        queue.pop(task);
        (*task)();
        delete task;
    }
};


#endif //ZMQEXECUTORLIB_TASKQUEUE_H
