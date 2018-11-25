#ifndef THREADPOOL
#define THREADPOOL

#include<iostream>
#include<queue>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<utility>
#include<shared_mutex>
#include<memory>
#include<future>
#include<functional>

class TaskQueue{
  public:
    typedef std::function<bool(void)> Task;
    typedef std::shared_ptr<Task> TaskPtr;
    typedef std::queue<Task> Queue;

    bool empty() {return _impl.empty();}
    void push(const Task & t){_impl.push(t);}
    void apply(){
      while(!_impl.empty()){
        (_impl.front())();
        _impl.pop();
      }
    }
  private:
    Queue _impl;
};


class Executor{
  public:
    typedef TaskQueue Queue;
    typedef Queue::Task Task;
    typedef Queue::TaskPtr TaskPtr;

    Executor(const Executor &)= delete;
    Executor(Executor &&)= delete;
    //constructor
    Executor(): running(true),
                _thread([this](){run();}){
               }

    void join();
    template <typename F, typename ...Args>
    auto task(F && f, Args && ...args)->std::future<decltype(f(args...))>{
      using ResType = decltype(f(args...));
      auto task = std::make_shared<std::packaged_task<ResType()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
      _task_list.push([task]()->bool{
          (*task)();
          return true;
      });
      cv.notify_one();
      return task->get_future();
    }
    virtual ~Executor();

  private:
    void run();
    std::condition_variable cv;
    std::mutex cv_m;
    Queue _task_list;
    volatile bool running;
    volatile bool stoped;
    std::thread  _thread;
};

#endif
