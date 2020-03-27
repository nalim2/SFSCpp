#ifndef ZMQLOOPLIB_TASKQUEUE_H
#define ZMQLOOPLIB_TASKQUEUE_H

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


#endif //ZMQLOOPLIB_TASKQUEUE_H
