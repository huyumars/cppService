#ifndef SOCKET
#define SOCKET

#include "NetUtil.H"


class Socket {
  public:
    typedef int SocketHandle;
    enum class SocketType:int {
      TCP = SOCK_STREAM,
    };
    enum {INVALID_SOCKET= -1};
    explicit Socket(SocketHandle handle);
    Socket();
    Socket(const SocketType &);
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&)=delete;
    Socket(Socket &&);
    virtual ~Socket();
    Socket& operator=(Socket&&);

    bool valid() const{return fd!=INVALID_SOCKET;};
    SocketHandle handler() const{return fd;}

    Socket accept() const;
    int setsockopt(int name, int value) const;
    int setNonBlocking() const;
    int setBlocking() const;
    int bind(const NetAddress & address) const;
    int listen(int backlog_=5) const;
    int connect(const NetAddress & address) const;
    inline ssize_t send(const void * buf, size_t len_) const;
    inline ssize_t recv(void * buf_, size_t len_) const;



  private:
    SocketHandle fd;
};


inline ssize_t Socket::send(const void * buf, size_t len) const {
  return ::send(fd,buf, len, 0);
}

inline ssize_t Socket::recv(void * buf_, size_t len) const{
  return ::recv(fd, buf_, len, 0);
}

#endif
