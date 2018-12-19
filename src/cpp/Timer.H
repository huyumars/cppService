#ifndef _TIMER
#define _TIMER
#include <unordered_map>
#include <queue>
#include <chrono>
#include <memory>
#include <mutex>
#include <functional>
#include "Defines.H"
#include "Types.H"

const int defaultTimeout = 1000;
enum class TimerType {
  Regular,
  Absolute,
  Repeat
};


class TimerMgr;
class TimerLoop;

class Timer {
  public:
    typedef std::shared_ptr<Timer>                  Ptr;
    typedef std::weak_ptr<Timer>                    WPtr;
    typedef std::chrono::system_clock::time_point   TimePoint;
    typedef std::chrono::system_clock::duration     Duration;
    typedef std::function<void (const Timer &)>     CallBack;

    virtual ~Timer() = default;
    Timer()=default;
    NOCOPYASS(Timer);
    Timer(Timer &&) = default;
    Timer & operator=(Timer &&)=default;


    int id() const {return _id;}
    TimePoint  expire() const {return _expire;}
    Duration   interval() const {return _interval;}
    std::string   name() const {return _name;}
    bool valid() const {return _loop.lock() && _cb;} ;
    void disable();
   

  protected:
    virtual void proccess() const;
    void enable(const std::weak_ptr<TimerMgr> &netloop);
    virtual TimerType type() const {return TimerType::Regular;};
    Timer(const std::string &,Duration, const CallBack &);

  private:
    int            _id;
    TimePoint      _expire;
    Duration       _interval;
    CallBack       _cb;
    std::string    _name;
    std::weak_ptr<TimerMgr>  _loop;
    friend TimerMgr;
    friend TimerLoop;

};


class TimerMgr :public SharedObject<TimerMgr>{
  public:
    void removeTimer(const std::string&);
    void processTimer();
    Timer::WPtr  topTimer() const;
    TimerMgr();
    const Timer & getTimer(const std::string &) const;
  private:
    bool _addTimer(const Timer::Ptr &timer);
    std::unordered_map<std::string,Timer::Ptr>  time_map;
    std::priority_queue<Timer::Ptr,
                       std::vector<Timer::Ptr>,
                       std::function<bool(const Timer::Ptr &, const Timer::Ptr &)>>  expireQueue;
    std::mutex     _timermgr_mutex;
    std::weak_ptr<TimerLoop>   _loop;
    friend TimerLoop;
};

class TimerLoop{
  public:
    TimerLoop();
    TimerMgr::WPtr  timerMgr() {return _mgr;}
    template<typename T, typename ...Args>
    std::weak_ptr<T> addTimerT(Args ...args){
      auto timer =  std::shared_ptr<T>(new T(std::forward<Args>(args)...));
      bool needweakup = need_weakup(*timer);
      bool ret = _mgr->_addTimer(std::dynamic_pointer_cast<Timer>(timer));
      if(ret){
        if(needweakup)
          addtimer_weakup();
        return timer;
      }
      else return std::weak_ptr<T>();
    }
    void processTimer() {
      _mgr->processTimer();
    }
    void removeTimer(const std::string name) {
      _mgr->removeTimer(name);
    }
    //in milliseconds
    int timeout_for_next_events();
  protected:
    virtual std::weak_ptr<TimerLoop> get_weak_ptr_of_time_loop() const = 0;
    virtual void addtimer_weakup() = 0;
  private:
    bool need_weakup(const Timer &) const;
    TimerMgr::Ptr  _mgr;
};

#endif
