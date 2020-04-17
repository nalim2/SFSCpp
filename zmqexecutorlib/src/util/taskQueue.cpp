#include "taskQueue.h"

TaskQueue::TaskQueue() : queue(boost::lockfree::queue < std::function < void() > * > (0)) {}

void TaskQueue::execute() {
    std::function<void()> *task;
    queue.pop(task);
    (*task)();
    delete task;
}

void TaskQueue::add(std::function<void()> *task) {
    queue.push(task);
}

