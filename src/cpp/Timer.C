#include "Timer.H"
#include "Logger.H"


Timer::Timer(const std::string & name,
             Duration   dur,
             const CallBack & cb):
  _expire(std::chrono::system_clock::now()+dur),
  _interval(dur),
  _cb(cb),
  _name(name)
{ }

void Timer::enable(const std::weak_ptr<TimerMgr> & loop_){
  _loop = loop_;
}

void Timer::proccess() const {
  if(_cb) _cb(*this);
}

void Timer::disable() {
  auto loop = _loop.lock();
  if(loop){
    loop->removeTimer(_name);
  }
}


TimerMgr::TimerMgr():
  expireQueue([](const Timer::Ptr & t1, const Timer::Ptr & t2){
      return t1->expire()>t2->expire();
      })
{}


bool TimerMgr::_addTimer(const Timer::Ptr &timer){
  std::unique_lock<std::mutex> lock(_timermgr_mutex);
  if(time_map.count(timer->name())){
    return false;
  }
  time_map[timer->name()] = timer;
  expireQueue.push(timer);
  timer->enable(this->get_weak());
  return true;
}


void TimerMgr::removeTimer(const std::string &name) {
  std::unique_lock<std::mutex> lock(_timermgr_mutex);
  if(time_map.count(name)){
    // use empty function
    time_map[name]->_cb =Timer::CallBack();
  }
}

void TimerMgr::processTimer() {
  std::unique_lock<std::mutex> lock(_timermgr_mutex);
  auto current_time = std::chrono::system_clock::now();
  while(expireQueue.size()){
    auto timer = expireQueue.top();
    if(timer->expire()<=current_time){
      timer->proccess();
      time_map.erase(timer->name());
      expireQueue.pop();
    }
    else break;
  }
}

Timer::WPtr TimerMgr::topTimer() const {
  if(expireQueue.size())
    return (expireQueue.top());
  else return Timer::WPtr();
}

const Timer & TimerMgr::getTimer(const std::string &name) const{
  return *(time_map.at(name));
}



TimerLoop::TimerLoop()
 :_mgr(TimerMgr::Factory::create()){
}

int TimerLoop::timeout_for_next_events(){
  if(_mgr->expireQueue.size()==0) return defaultTimeout;
  auto  top = _mgr->topTimer().lock();
  if(top){
    auto dur = top->expire() - std::chrono::system_clock::now();
    int timeout = dur.count()/(1000000) ;
    return std::min(timeout,defaultTimeout);
  }
  else return defaultTimeout;
}

bool TimerLoop::need_weakup(const Timer & timer) const {
  auto top = _mgr->topTimer().lock();
  if(top){
    if(top->expire() > timer.expire()) return true;
    else return false;
  }
  //if no timer before need wake up
  return true;
}

