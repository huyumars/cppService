#ifndef _TCPCONNECTION
#define _TCPCONNECTION
#include "NetLoop.H"
#include "NetChannel.H"


class TCPConnection {
  public:
    typedef std::string  Data;
    typedef threads::PCQueue<Data>  PCQueue;
    typedef std::function<void(const Data &data)>   ReadCallback;
    typedef std::function<void(TCPConnection & )>   ErrorCallBack;

    TCPConnection(NetLoop::Ptr netloop) ;
    void connect(Socket && client);
    std::string name() const {return _name;}
    void asyncSend(const Data & data);
    void setReadCallBack(const ReadCallback & rcb){_rcb = rcb;};
    void setErrorCallBack(const ErrorCallBack& ecb){_ecb = ecb;};
    void close();


  private:
    void  readHandle(const NetChannel & nc);
    void  writeHandle(const NetChannel & nc);
    void  errorHandle(const NetChannel & nc);
    std::string      _name;
    Socket           _socket;
    PCQueue          _writeQueue;
    ReadCallback     _rcb;
    ErrorCallBack    _ecb;
    NetChannel::Ptr _wchannel;
    NetChannel::Ptr _rchannel;
    NetChannel::Ptr _echannel;
    NetLoop::WPtr    _netloop;

};

#endif
