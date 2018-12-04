#ifndef SOCKET
#define SOCKET

#include "NetUtil.H"


class Socket {
  public:
    typedef int SocketHandle;

    enum {INVALID_SOCKET= -1};
    explicit Socket(SocketHandle handle);
    Socket();
    Socket(const NetAddress &);
    virtual ~Socket();
    bool valid() const{return fd==INVALID_SOCKET;};
    SocketHandle handler() const{return fd;}

    Socket accept() const;
    int bind(const NetAddress & address) const;
    int listen(int backlog_=5) const;
    int connect(const NetAddress & address) const;
    inline ssize_t send(const void * buf, size_t len_);
    inline ssize_t recv(void * buf_, size_t len_);
    int close();



  private:
    SocketHandle fd;
};

#endif
