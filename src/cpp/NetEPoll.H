#ifndef NETEPOLL
#define NETEPOLL

#include <sys/epoll.h>
#include "ThreadSafe.H"
#include "Socket.H"
#include <string.h>

class NetChannel;

class NetEPoll{
  public:
    enum {INVALID_EPOLL=-1};
    typedef int EPollHandle;
    typedef std::weak_ptr<NetChannel>       ChannelWPtr;
    typedef std::shared_ptr<NetChannel>     ChannelPtr;
    typedef std::vector<ChannelWPtr>        ChannelList;
    typedef std::unique_ptr<epoll_event>    EventPtr;

    NetEPoll();
    ~NetEPoll();
    //no copy
    NetEPoll(const NetEPoll&)=delete;
    //can move
    NetEPoll(NetEPoll &&);

    bool valid() const {return _epfd != INVALID_EPOLL;}
    void addChannel(const ChannelWPtr &) ;
    void rmChannel(const ChannelWPtr &) ;
    int multiplex(int timeout) ;
    void pendingList(ChannelList &list_) ;
  private:
    struct EPollData{
      EventPtr      eventPtr;     //even ptr
      ChannelWPtr    rchannelptr;  //read
      ChannelWPtr    wchannelptr;  //write
      ChannelWPtr    echannelptr;  //error
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
    std::mutex                                    _pendingEventsMutex;

};


#endif
