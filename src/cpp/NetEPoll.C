#include "NetEPoll.H"
#include <unistd.h>


NetEPoll::NetEPoll():
  _epfd( ::epoll_create1(0)),
  _numpendingEvents(0){
    // might set to EPOLL_CLOEXEC which will gurantee fd will be closed after fork()
}


NetEPoll::~NetEPoll(){
  ::close(_epfd);
}

NetEPoll::NetEPoll(NetEPoll && epoll)
  :_epfd(epoll._epfd){
  epoll._epfd=INVALID_EPOLL;
}


void NetEPoll::addChannel(const ChannelPtr&cp){
  Socket::SocketHandle fd = cp->fd();
  if(!_registerSockets.count(fd)){
    _registerSockets[fd] = EPollData();
    _registerSockets[fd].eventPtr->data.fd = fd;
  }
  EPollData & polldata = _registerSockets[fd];
  switch(cp->type()){
    case NetChannel::Type::Read:
      polldata.rchannelptr = cp;
      polldata.eventPtr->events |= EPOLLIN;
      break;
    case NetChannel::Type::Write:
      polldata.wchannelptr = cp;
      polldata.eventPtr->events |= EPOLLOUT;
      break;
    case NetChannel::Type::Error:
      polldata.echannelptr = cp;
      polldata.eventPtr->events |= EPOLLERR;
      break;
    default:
      break;
  }
  ::epoll_ctl(_epfd,EPOLL_CTL_ADD,cp->fd(),polldata.eventPtr.get());
}

void NetEPoll::rmChannel(const ChannelPtr&cp){
  Socket::SocketHandle fd = cp->fd();
  if(!_registerSockets.count(fd)) return;
  EPollData & polldata = _registerSockets[fd];
  switch(cp->type()){
    case NetChannel::Type::Read:
      polldata.eventPtr->events -= EPOLLIN;
      break;
    case NetChannel::Type::Write:
      polldata.eventPtr->events -= EPOLLOUT;
      break;
    case NetChannel::Type::Error:
      polldata.eventPtr->events -= EPOLLERR;
      break;
    default:
      break;
  }
  ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,polldata.eventPtr.get());
  auto events = polldata.eventPtr->events;
  //no events anymore
  if(events == 0){
    _registerSockets.erase(fd);
  }
}


int NetEPoll::multiplex(int timeout){
  memset(_pendingEvents,0,sizeof(epoll_event)*_maxPoll);
  int _numpendingEvents= ::epoll_wait(_epfd,_pendingEvents,_maxPoll,timeout);
  return _numpendingEvents;
}

void NetEPoll::getPendingChannelList(ChannelList & list_){
  //clean
  list_.clear();
  for(int i=0; i< _numpendingEvents; ++i){

  }

}

