#include "NetLoop.H"
#include "NetChannel.H"

NetLoop::NetLoop():
  running(true)
{ }

NetLoop::~NetLoop(){
  //_wakeupchannel must delete manually
  rmChannel(*_wakeupchannel.get());
}


void NetLoop::runLoop(){
  _loopthread_id = std::this_thread::get_id();
  if(get_weak().expired()) throw NetException("netloop object didn't set up ");
  _wakeupchannel  =  WakeUpChannel::Factory::create(get_weak().lock(),[](const NetChannel &){
      });
  while(running){
    ioMulitplex(1000);
    processChannels();
  }
}

void NetLoop::start(){
  runLoop();
}

void NetLoop::ioMulitplex(int timeout){
  poller.multiplex(timeout);
}

void NetLoop::processChannels(){
  pendingChannels.clear();
  poller.pendingList(pendingChannels);
  for(Poller::ChannelWPtr ptr: pendingChannels){
    if(auto channelptr = ptr.lock())
      channelptr->process();
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
