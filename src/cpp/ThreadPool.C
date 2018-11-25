#include "ThreadPool.H"
#include <chrono>

void Executor::join(){
  _thread.join();
}

void Executor::run(){
  while(running){
    std::unique_lock<std::mutex> lock(cv_m);
    cv.wait(lock,[this]{return !_task_list.empty();});
    _task_list.apply();
  }
}


Executor::~Executor(){
  running = false;
  join();
}
