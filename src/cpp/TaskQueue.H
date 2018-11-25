#ifndef TASKQUEUE
#define TASKQUEUE

#include <shared_mutex>
#include <memory>
#include <queue>

template<typename T>
class SafeQueue{
  public:
    typedef std::shared_lock<std::shared_timed_mutex> ReadLock;
    typedef std::unique_lock<std::shared_timed_mutex> WriteLock;
    bool empty(){
      ReadLock lock(mutex);
      return _impl.empty();
    }
    const T & front(){
      ReadLock lock(mutex);
      return _impl.front();
    }
    void push(const T &t){
      WriteLock lock(mutex);
      _impl.push(t);
    }
    void pop(){
      WriteLock lock(mutex);
      _impl.pop();
    }
  private:
    std::queue<T> _impl;
    std::shared_timed_mutex mutex;
};



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
