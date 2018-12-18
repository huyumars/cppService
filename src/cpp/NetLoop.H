#ifndef NETLOOP
#define NETLOOP
#include <memory>
#include <map>
#include <atomic>
#include <thread>
#include <functional>
#include "NetEPoll.H"
#include "Types.H"
#include "Timer.H"


class WakeUpChannel;
class NetChannel;

class NetLoop :public SharedObject<NetLoop>, public TimerLoop{
  public:
    typedef std::unique_ptr<NetLoop> UPtr;
    typedef std::thread::id              ThreadID;
    
    //for linux
    typedef  NetEPoll          Poller;
    typedef  NetEPoll::ChannelWPtr  ChannelWPtr;
    typedef  NetEPoll::ChannelPtr   ChannelPtr;
    typedef  NetEPoll::ChannelList  ChannelList;
    typedef  std::function<void(const WakeUpChannel&)> WKChannelCB;
    typedef  std::function<void(const NetLoop&)>    WakeUpCB;

    void start();
    bool inloopThread() const {return std::this_thread::get_id()==_loopthread_id;}
    void asyncStop();
    void asyncWakeUp(const WakeUpCB & wakeup_cb);
    bool addChannel(const NetChannel&);
    bool rmChannel(const NetChannel&);
    ~NetLoop();

  private:
    NetLoop();
    //only allow factory to create
    friend   SharedObject<NetLoop>::Factory;
    void stop(){running = false;}
    void runLoop();
    void ioMulitplex(int timeout);
    void processChannels();
    void processWakeUp(const WakeUpChannel &);

    //time related
    std::weak_ptr<TimerLoop> get_weak_ptr_of_time_loop() const override;

    std::atomic_bool      running;
    Poller                poller;
    ChannelList           pendingChannels;
    ThreadID               _loopthread_id;
    std::shared_ptr<WakeUpChannel> _wakeupchannel;
    WKChannelCB           _wakeup_callBack;
    threads::PCQueue<WakeUpCB>  _wakeup_cb_queue;
};


#endif
