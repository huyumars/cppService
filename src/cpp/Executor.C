#include "Executor.H"
#include <chrono>
#include "Logger.H"

void Executor::join(){
  _thread.join();
}

void Executor::run(){
  LogINFO << "executor start at"<<std::this_thread::get_id()<<LogSend;
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
  LogINFO << "executor stoped at "<<std::this_thread::get_id()<<LogSend;
}
