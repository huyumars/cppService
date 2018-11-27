#ifndef THREADSAFE
#define THREADSAFE


#include <queue>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>

//product consume queue
template<typename T>
class PCQueue{
  public:
    typedef std::queue<T>  QueueType;
    typedef std::shared_ptr<QueueType>  QueuePtr;


    PCQueue():running(true){}
    void put(const T &t){
      std::unique_lock<std::mutex> lock(mutex);
      _impl.push(t);
      lock.unlock();
      cv.notify_all();
    }
    void getQueue(QueuePtr &  queuePtr_){
      queuePtr_ = std::make_shared<QueueType>();
      std::unique_lock<std::mutex> lock(mutex);
      while(running && _impl.empty()){
        cv.wait(lock);
      }
      queuePtr_->swap(_impl);
    }
    T get(){
      std::unique_lock<std::mutex> lock(mutex);
      while(running && _impl.empty()){
        cv.wait(lock);
      }
      auto t = _impl.front();
      _impl.pop();
      return t;
    }
    void stop(){
      running = false;
      cv.notify_all();
    }
    ~PCQueue(){
      stop();
    }
  private:
    QueueType      _impl;
    std::mutex    mutex;
    std::condition_variable cv;
    std::atomic_bool running;
};

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
