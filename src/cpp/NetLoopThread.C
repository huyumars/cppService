
#include "NetLoopThread.H"

NetLoopThread::NetLoopThread(){}

void NetLoopThread::start(bool block){
  _thread = std::thread([this](){
      run();
  });
  if(block){
    std::unique_lock<std::mutex> locker(_start_mt);
    _start_cv.wait_for(locker, std::chrono::seconds(1));
  }
}

void NetLoopThread::run() {
   _netloop = NetLoop::Factory::create();
   _start_cv.notify_all();
   _netloop->start();
}


void NetLoopThread::stop(bool imme){
  _netloop->asyncStop();
}
