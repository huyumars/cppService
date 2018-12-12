#include "NetEPoll.H"
#include  "NetChannel.H"
#include <unistd.h>


NetEPoll::NetEPoll():
  // might set to EPOLL_CLOEXEC which will gurantee fd will be closed after fork()
  _epfd( ::epoll_create1(0)),
  _numpendingEvents(0){
}


NetEPoll::~NetEPoll(){
  ::close(_epfd);
}

NetEPoll::NetEPoll(NetEPoll && epoll)
  :_epfd(epoll._epfd){
  epoll._epfd=INVALID_EPOLL;
}


void NetEPoll::addChannel(const NetChannel &ch){
  Socket::SocketHandle fd = ch.fd();
  int op = EPOLL_CTL_MOD;
  if(!_registerSockets.count(fd)){
    _registerSockets[fd] = EPollData();
    _registerSockets[fd].eventPtr->data.fd = fd;
    op = EPOLL_CTL_ADD;
  }
  EPollData & polldata = _registerSockets[fd];
  switch(ch.type()){
    case NetChannel::Type::Read:
      polldata.rchannelptr = ch.get_weak();
      polldata.eventPtr->events |= (EPOLLIN|EPOLLHUP);
      break;
    case NetChannel::Type::Write:
      polldata.wchannelptr = ch.get_weak();
      polldata.eventPtr->events |= EPOLLOUT;
      break;
    case NetChannel::Type::Error:
      polldata.echannelptr = ch.get_weak();
      polldata.eventPtr->events |= EPOLLERR;
      break;
    default:
      break;
  }
  ::epoll_ctl(_epfd,op,ch.fd(),polldata.eventPtr.get());
}

void NetEPoll::rmChannel(const NetChannel &ch){
  Socket::SocketHandle fd = ch.fd();
  if(!_registerSockets.count(fd)) return;
  EPollData & polldata = _registerSockets[fd];
  switch(ch.type()){
    case NetChannel::Type::Read:
      polldata.eventPtr->events &= ~(EPOLLIN|EPOLLHUP);
      break;
    case NetChannel::Type::Write:
      polldata.eventPtr->events &= ~EPOLLOUT;
      break;
    case NetChannel::Type::Error:
      polldata.eventPtr->events &= ~EPOLLERR;
      break;
    default:
      break;
  }
  //no events anymore delete
  if(polldata.eventPtr->events== 0){
    ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,polldata.eventPtr.get());
    _registerSockets.erase(fd);
  }
  // just modify
  else {
    ::epoll_ctl(_epfd,EPOLL_CTL_MOD,fd,polldata.eventPtr.get());
  }
}


int NetEPoll::multiplex(int timeout){
  memset(_pendingEvents,0,sizeof(epoll_event)*_maxPoll);
  int rc= ::epoll_wait(_epfd,_pendingEvents,_maxPoll,timeout);
  _numpendingEvents = std::max(0,rc);
  return rc;
}

void NetEPoll::pendingList(ChannelList & list_){
  //clean
  std::unique_lock<std::mutex> pendingLock(_pendingEventsMutex);
  for(int i=0; i< _numpendingEvents; ++i){
    Socket::SocketHandle fd = _pendingEvents[i].data.fd; 
    EPollData & polldata = _registerSockets[fd];
    if(_pendingEvents[i].events &(EPOLLIN|EPOLLHUP)){
       list_.push_back(polldata.rchannelptr);
    }
    if(_pendingEvents[i].events &(EPOLLOUT)){
       list_.push_back(polldata.wchannelptr);
    }
    if(_pendingEvents[i].events &(EPOLLERR)){
       list_.push_back(polldata.echannelptr);
    }
  }
}

