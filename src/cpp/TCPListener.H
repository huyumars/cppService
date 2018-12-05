#ifndef TCPLISTENER
#define TCPLISTENER

#include "Socket.H"
#include <functional>

class TCPListerner{
  public:
    typedef std::function<void(const Socket &)> AcceptCallBack;

    TCPListerner(const NetAddress &);
    //copy
    TCPListerner(const TCPListerner &) = default;
    //move
    TCPListerner(TCPListerner &&) = default;
    virtual ~TCPListerner(){}

    int startlisten();
    int valid() const;
    void setAcceptCallBack(const AcceptCallBack & acb);


  private:
    NetAddress addr;
    Socket socket;
    AcceptCallBack acb;
};


#endif
