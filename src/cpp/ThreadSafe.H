#ifndef THREADSAFE
#define THREADSAFE


#include <queue>
#include <shared_mutex>

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


template <class Class>
class Singleton{
  public:
    static Class& instance(){
      static Class _inst;
      return _inst;
    }
    Singleton(const Singleton &)= delete;
    Singleton(Singleton && )=delete;
    Singleton() = default;
    ~Singleton() = default;
};

#endif
