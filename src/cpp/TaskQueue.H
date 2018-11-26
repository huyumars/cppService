#ifndef TASKQUEUE
#define TASKQUEUE

#include "ThreadSafe.H"
#include <memory>


class TaskQueue{
  public:
    typedef std::function<bool(void)> Task;
    typedef std::shared_ptr<Task> TaskPtr;
    typedef SafeQueue<Task> Queue;

    inline bool empty() {return _impl.empty();}
    inline void push(const Task & t){_impl.push(t);}
    void apply();

  private:
    Queue _impl;
};

#endif
