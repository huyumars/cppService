#ifndef TCPCLIENT
#define TCPCLIENT

#include "NetUtil.H"
#include "Socket.H"
#include <functional>


class TCPClientConnection{
  public:
    typedef std::string Connection;
    typedef std::function<void(const Connection &, const std::string &)> CallBackFunc;
    TCPClientConnection();
    TCPClientConnection(const TCPClientConnection &)= delete;
    TCPClientConnection(TCPClientConnection &&) = delete;
    ~TCPClientConnection();

    void connect(const NetAddress & address);
    bool send(const std::string & msg);
    void setCallBack(const CallBackFunc  & cb);
  private:
    NetAddress  address;
    Socket       socket;
    CallBackFunc _callback;
};

#endif
