#ifndef NETLOOP
#define NETLOOP
#include <memory>
#include <map>
#include <atomic>
#include <thread>
#include "NetEPoll.H"
#include "Types.H"


class WakeUpChannel;

class NetLoop :public SharedObject<NetLoop>{
  public:
    typedef std::unique_ptr<NetLoop> UPtr;
    typedef std::thread::id              ThreadID;
    
    //for linux
    typedef  NetEPoll          Poller;
    typedef  NetEPoll::ChannelWPtr  ChannelWPtr;
    typedef  NetEPoll::ChannelPtr   ChannelPtr;
    typedef  NetEPoll::ChannelList  ChannelList;

    void start();
    bool inloopThread() const {return std::this_thread::get_id()==_loopthread_id;}
    void stop(){running = false;}
    bool addChannel(const NetChannel&);
    bool rmChannel(const NetChannel&);
    ~NetLoop();

  private:
    NetLoop();
    //only allow factory to create
    friend   SharedObject<NetLoop>::Factory;
    void runLoop();
    void ioMulitplex(int timeout);
    void processChannels();
    std::atomic_bool      running;
    Poller                poller;
    ChannelList           pendingChannels;
    ThreadID               _loopthread_id;
    std::shared_ptr<WakeUpChannel> _wakeupchannel;
};


#endif
