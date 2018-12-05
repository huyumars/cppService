#ifndef NETLOOP
#define NETLOOP
#include <memory>
#include <map>

class NetChannel;


class NetLoop {
  public:
    typedef std::shared_ptr<NetLoop> Ptr;
    typedef std::unique_ptr<NetLoop> UPtr;
    typedef std::weak_ptr<NetLoop> WPtr;
    typedef std::unique_ptr<NetChannel> NetChannelUPtr;

    NetLoop()=default;

  private:
    void runLoop();
    void ioMulitplex(int timeout);
    std::map<std::string,NetChannelUPtr> _netchannel_list;

};


#endif
