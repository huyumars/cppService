#ifndef _NETLOOPTHREAD
#define _NETLOOPTHREAD

#include "NetLoop.H"
#include <mutex>
#include <condition_variable>

class NetLoopThread {
  public:
    NetLoopThread();
    //wait whole thread setup
    void start(bool block=true);
    //imme = true: withdraw everything
    //=false , process all everythins then stop
    void stop(bool immediately=false);
    void join() { if(_thread.joinable())_thread.join();}
    NetLoop::Ptr   netloop() const {return _netloop;}
    //no copy
    NetLoopThread(const NetLoopThread & ) = delete;
  private:
    void run();
    NetLoop::Ptr              _netloop;
    std::thread               _thread;
    std::condition_variable   _start_cv;
    std::mutex                _start_mt;
};

#endif
