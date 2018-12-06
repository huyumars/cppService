#ifndef NETEPOLL
#define NETEPOLL

#include <sys/epoll.h>
#include "ThreadSafe.H"
#include "NetChannel.H"

class NetEPoll {
  public:
    typedef int EPollHandle;
    typedef std::shared_ptr<NetChannel>  ChannelPtr;
    //static data won't be managered
    typedef struct epoll_event *         EventRPtr;

    NetEPoll();
    ~NetEPoll();
  private:
    struct EPollData{
      EventRPtr     eventPtr;     //even ptr
      ChannelPtr    rchannelptr;  //read
      ChannelPtr    wchannelptr;  //write
      ChannelPtr    echannelptr;  //error
    };

    static const int _maxPoll= 10;
    EPollHandle         _epfd;
    int                 _numpendingEvents;
    struct epoll_event  _pendingEvents[_maxPoll];

};


#endif
