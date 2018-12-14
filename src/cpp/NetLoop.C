#include "NetLoop.H"
#include "NetChannel.H"
#include "Logger.H"

NetLoop::NetLoop():
  running(true)
{ }

NetLoop::~NetLoop(){
  //_wakeupchannel must delete manually
  rmChannel(*_wakeupchannel.get());
  LogDEBUG<<"stoped netloop "<<LogSend;
}



void NetLoop::processWakeUp(const WakeUpChannel & ch){
  ch.ack();
  LogDEBUG<<"netloop wakeup by eventfd "<<ch.fd()<<LogSend;
  threads::PCQueue<WakeUpCB>::QueuePtr cbs;
  _wakeup_cb_queue.tryGet(cbs);
  while(!(cbs->empty())){
    auto cb = cbs->front();
    cbs->pop();
    cb(*this);
  }
  if(_wakeup_callBack) _wakeup_callBack(ch);
}

void NetLoop::runLoop(){
  _loopthread_id = std::this_thread::get_id();
  if(get_weak().expired()) throw NetException("netloop object didn't set up ");
  _wakeupchannel  =  WakeUpChannel::Factory::create(get_weak().lock(), [this](const NetChannel &ch){
      auto p = dynamic_cast<const WakeUpChannel *>(&ch);
      processWakeUp(*p);
  });
  _wakeupchannel->enable();
  while(running){
    ioMulitplex(1000);
    processChannels();
  }
}

void NetLoop::start(){
  runLoop();
}

void NetLoop::asyncStop(){
  if(inloopThread()) return;
  _wakeup_callBack = [this](const WakeUpChannel &){
    LogDEBUG<<"stoping netloop "<<LogSend;
    stop();
  };
  _wakeupchannel->notify();
}

void NetLoop::asyncWakeUp(const WakeUpCB &cb){
  if(inloopThread()) return;
  _wakeup_cb_queue.put(cb);
  _wakeupchannel->notify();
}

void NetLoop::ioMulitplex(int timeout){
  poller.multiplex(timeout);
}

void NetLoop::processChannels(){
  pendingChannels.clear();
  poller.pendingList(pendingChannels);
  for(Poller::ChannelWPtr ptr: pendingChannels){
    if(auto channelptr = ptr.lock()){
      channelptr->process();
    }
  }
}


bool NetLoop::addChannel(const NetChannel& cp){
  poller.addChannel(cp);
  return true;
}

bool NetLoop::rmChannel(const NetChannel&cp){
  poller.rmChannel(cp);
  return true;
}
