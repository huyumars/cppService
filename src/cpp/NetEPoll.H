#ifndef NETEPOLL
#define NETEPOLL

#include <sys/epoll.h>
#include "ThreadSafe.H"
#include "Socket.H"
#include "NetChannel.H"
#include <string.h>

class NetEPoll {
  public:
    enum {INVALID_EPOLL=-1};
    typedef int EPollHandle;
    typedef std::shared_ptr<NetChannel>  ChannelPtr;
    typedef std::vector<ChannelPtr>      ChannelList; 
    typedef std::unique_ptr<epoll_event> EventPtr;

    NetEPoll();
    ~NetEPoll();
    //no copy
    NetEPoll(const NetEPoll&)=delete;
    //can move
    NetEPoll(NetEPoll &&);

    bool valid() const {return _epfd != INVALID_EPOLL;}
    void addChannel(const ChannelPtr &);
    void rmChannel(const ChannelPtr &);
    int multiplex(int timeout) ;
    void getPendingChannelList(ChannelList &list_);
  private:
    struct EPollData{
      EventPtr      eventPtr;     //even ptr
      ChannelPtr    rchannelptr;  //read
      ChannelPtr    wchannelptr;  //write
      ChannelPtr    echannelptr;  //error
      EPollData():eventPtr(std::make_unique<epoll_event>()){
        // set all to zero
        ::memset(eventPtr.get(),0,sizeof(epoll_event));
      }
    };


    static const int _maxPoll= 10;
    EPollHandle                                   _epfd;
    int                                           _numpendingEvents;
    struct epoll_event                            _pendingEvents[_maxPoll];
    threads::Map<Socket::SocketHandle,EPollData>  _registerSockets;

};


#endif
