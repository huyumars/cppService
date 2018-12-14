#ifndef THREADSAFE
#define THREADSAFE


#include <queue>
#include <shared_mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>

namespace  threads {
//product consume queue
template<typename T, typename QueueType = std::queue<T>>
class PCQueue{
  public:
    typedef std::shared_ptr<QueueType>  QueuePtr;

    PCQueue():running(true){}
    //unsafe get empty()
    bool _empty() const {return _impl.empty();}
    void put(const T &t){
      std::unique_lock<std::mutex> lock(mutex);
      _impl.push(t);
      lock.unlock();
      cv.notify_all();
    }
    void put(T &&t){
      std::unique_lock<std::mutex> lock(mutex);
      _impl.push(std::move(t));
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
      auto t = std::move(_impl.front());
      _impl.pop();
      return t;
    }
    void tryGet(QueuePtr & queuePtr_){
      queuePtr_ = std::make_shared<QueueType>();
      std::unique_lock<std::mutex> lock(mutex);
      queuePtr_->swap(_impl);
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



template<typename K, typename T,typename M=std::unordered_map<K,T>>
class Map{
  public:
    typedef std::shared_lock<std::shared_timed_mutex> ReadLock;
    typedef std::unique_lock<std::shared_timed_mutex> WriteLock;
    bool empty(){
      ReadLock lock(mutex);
      return _impl.empty();
    }
    bool count(const K & key){
      ReadLock lock(mutex);
      return _impl.count(key);
    }
    const T& at(const K &key){
      ReadLock lock(mutex);
      return _impl.at(key);
    }
    //write
    T& operator[](const K &key){
      WriteLock lock(mutex);
      return _impl[key];
    }
    size_t erase(const K & key){
      WriteLock lock(mutex);
      return _impl.erase(key);
    }
  private:
    M  _impl;
    std::shared_timed_mutex mutex;
};

template<typename T, typename Q = std::queue<T>>
class Queue{
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
    Q _impl;
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


template <class T>
class ThreadLocal{};


template <class T>
class Singleton<ThreadLocal<T>>{
  public:
    static T& instance(){
      thread_local static T _inst;
      return _inst;
    }
    Singleton(const Singleton &)= delete;
    Singleton(Singleton && )=delete;
    Singleton() = default;

};

}

#endif

