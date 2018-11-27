#include "Executor.H"
#include <chrono>

void Executor::join(){
  _thread.join();
}

void Executor::run(){
  while(running){
    _task_list.get()();
  }
}

Executor::Executor():running(true),
                     _thread([this]{run();}){
                     }

Executor::~Executor(){
  _task_list.put([this](){
      running = false;
      return true;
      });
  join();
}
