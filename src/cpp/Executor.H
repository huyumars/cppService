#ifndef EXECUTOR
#define EXECUTOR

#include "TaskQueue.H"

#include<iostream>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<utility>
#include<memory>
#include<future>
#include<functional>

class Executor{
  public:
    typedef TaskQueue Queue;
    typedef Queue::Task Task;
    typedef Queue::TaskPtr TaskPtr;

    Executor(const Executor &)= delete;
    Executor(Executor &&)= delete;
    //constructor
    Executor();
    virtual ~Executor();
    void join();
    template <typename F, typename ...Args>
    auto task(F && f, Args && ...args)->std::future<decltype(f(args...))>{
      using ResType = decltype(f(args...));
      auto task =
        std::make_shared<std::packaged_task<ResType()>>
        (std::bind(std::forward<F>(f),std::forward<Args>(args)...));
      _task_list.push([task]()->bool{
          (*task)();
          return true;
      });
      cv.notify_one();
      return task->get_future();
    }

  private:
    void run();
    std::condition_variable cv;
    std::mutex cv_m;
    Queue _task_list;
    volatile bool running;
    std::thread  _thread;
};

#endif