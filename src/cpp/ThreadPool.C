#include "ThreadPool.H"

void Executor::join(){
  _thread.join();
}

void Executor::run(){
  while(true){
    std::unique_lock<std::mutex> lock(cv_m);
    cv.wait(lock,[this]{return !_task_list.empty();});
    _task_list.apply();
  }
}
