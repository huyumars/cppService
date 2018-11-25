#include "Executor.H"
#include <chrono>

void Executor::join(){
  _thread.join();
}

void Executor::run(){
  while(running){
    std::unique_lock<std::mutex> lock(cv_m);
    cv.wait(lock,[this]{return !running || !_task_list.empty();});
    _task_list.apply();
  }
}

Executor::Executor():running(true),
                     _thread([this]{run();}){
                     }

Executor::~Executor(){
  running = false;
  //without notify, will not exit forever
  cv.notify_one();
  join();
}
