#include "NetLoop.H"
#include "NetChannel.H"

NetLoop::NetLoop():
  running(true){}


void NetLoop::runLoop(){
  while(running){
    ioMulitplex(0);
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


bool NetLoop::addChannel(const ChannelWPtr & cp){
  poller.addChannel(cp);
  return true;
}

bool NetLoop::rmChannel(const ChannelWPtr &cp){
  poller.rmChannel(cp);
  return true;
}
