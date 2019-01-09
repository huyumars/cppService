#ifndef TCPLISTENER
#define TCPLISTENER

#include "Socket.H"
#include "NetLoop.H"
#include <functional>

class TCPListener{
  public:
    enum  class State: int{
      invalid,
      listen,
      close,
      error
    };
    typedef std::weak_ptr<NetLoop>  NetLoopWPtr;
    typedef std::shared_ptr<NetChannel>  NetChannelPtr;
    typedef std::function<void(Socket &&)> AcceptCallBack;

    TCPListener(NetLoop::Ptr netloop, const NetAddress &);
    TCPListener(const TCPListener &) = delete;
    TCPListener(TCPListener &&) = delete;

    virtual ~TCPListener(){}

    bool listen();
    std::string name() const { return "listener";};
    State state() const {return _state;}
    void setAcceptCallBack(const AcceptCallBack & acb){_acb = acb;};


  private:
    void acceptHandle(const NetChannel & nc);
    State           _state;
    NetLoopWPtr     _netloop;
    NetAddress      _addr;
    Socket          _socket;
    AcceptCallBack  _acb;
    NetChannelPtr   _listenChannel;
};


#endif
