#ifndef NETLOOP
#define NETLOOP
#include <memory>
#include <map>
#include <atomic>
#include "NetEPoll.H"


class Poller;


class NetLoop {
  public:
    typedef std::shared_ptr<NetLoop> Ptr;
    typedef std::unique_ptr<NetLoop> UPtr;
    typedef std::weak_ptr<NetLoop> WPtr;
    
    //for linux
    typedef  NetEPoll          Poller;
    typedef  NetEPoll::ChannelWPtr  ChannelWPtr;
    typedef  NetEPoll::ChannelPtr   ChannelPtr;
    typedef  NetEPoll::ChannelList  ChannelList;

    NetLoop();
    void start();
    void stop(){running = false;}
    bool addChannel(const ChannelWPtr&);
    bool rmChannel(const ChannelWPtr&);

  private:
    void runLoop();
    void ioMulitplex(int timeout);
    void processChannels();
    std::atomic_bool      running;
    Poller                poller;
    ChannelList           pendingChannels;
};


#endif
