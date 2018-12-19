#include <gmock/gmock.h>
#include "testUtility.H"
#include <condition_variable>
#include "Logger.H"
#include <mutex>
#include <thread>
#include "timerTest.H"
#include "Timer.H"
#include "Types.H"

class testTimeLoop :public SharedObject<testTimeLoop>, public TimerLoop {
  public:
    testTimeLoop():running(true),thd([this](){runloop();}){ }
    void stop(){
      running = false;
      cv.notify_all();
    }
    ~testTimeLoop(){
      stop();
      thd.join();
    }
  private:
    std::weak_ptr<TimerLoop> get_weak_ptr_of_time_loop() const override {
      return std::dynamic_pointer_cast<TimerLoop>(get_weak().lock());
    }
    void addtimer_weakup() override{
      cv.notify_all();
    }
    void runloop(){
      while(running){
        int timeout = this->timeout_for_next_events();
        std::unique_lock<std::mutex>  locker(mt);
        cv.wait_for(locker,std::chrono::milliseconds(timeout));
        this->processTimer();
      }
    }
    bool                      running;
    std::thread               thd;
    std::condition_variable   cv;
    std::mutex                mt;
};


TEST_F(TimerTest, addtimerTest){
  IntCallback tcb;
  testing::Sequence  seq;
  for(int i=0;i<10;++i){
     EXPECT_CALL(tcb,val(::testing::Eq(i))).InSequence(seq);
  }
  auto loop = testTimeLoop::Factory::create();
  for(int i =9; i>=0; --i){
    loop->addTimerT<Timer>("timer"+std::to_string(i), std::chrono::milliseconds(i*100),[i, &tcb](const Timer &){
      LogINFO<<"timer "<<i<<" trigger"<<LogSend;
      tcb.val(i);
      });
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1300));
}

TEST_F(TimerTest, rmTimerTest){
  IntCallback tcb;
  testing::Sequence  seq;
  for(int i=1;i<10;i+=2){
     EXPECT_CALL(tcb,val(::testing::Eq(i))).InSequence(seq);
  }
  auto loop = testTimeLoop::Factory::create();
  for(int i =10; i>=0; --i){
    loop->addTimerT<Timer>("timer"+std::to_string(i), std::chrono::milliseconds(i*100),[i,&tcb](const Timer &){
      LogINFO<<"timer "<<i<<" trigger"<<LogSend;
      tcb.val(i);
      });
  }
  for(int i=0; i<=10; i+=2){
    loop->removeTimer("timer"+std::to_string(i));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1300));
}
