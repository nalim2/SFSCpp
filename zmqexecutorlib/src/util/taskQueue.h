#ifndef ZMQEXECUTORLIB_TASKQUEUE_H
#define ZMQEXECUTORLIB_TASKQUEUE_H

#include <boost/lockfree/queue.hpp>
#include <iostream>

class TaskQueue {
private:
    boost::lockfree::queue<std::function<void()> *> queue;
public:
    TaskQueue();

    void add(std::function<void()> *task);

    void execute();
};


#endif //ZMQEXECUTORLIB_TASKQUEUE_H
