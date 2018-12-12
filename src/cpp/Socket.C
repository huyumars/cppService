#include "Socket.H"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include "Logger.H"

#define THROW_WHEN(ret,val,msg) \
  if(ret==val) throw NetException(msg);

#define VERIFY_FD \
  THROW_WHEN(fd,INVALID_SOCKET,"create socket failed");

Socket::Socket():fd(Socket::INVALID_SOCKET){}

Socket::Socket(Socket&& s):fd(s.fd){
  s.fd = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket&&s){
  if(fd!=INVALID_SOCKET) ::close(fd);
  fd=s.fd;
  s.fd = INVALID_SOCKET;
  return *this;
}

Socket::Socket(SocketHandle handle){
  ::close(fd);
  fd = handle;
}


Socket::Socket(const SocketType & type){
  fd = ::socket(AF_INET, static_cast<int>(type), 0);
  VERIFY_FD;
  LogDEBUG<<"start socket "<<fd<<LogSend;
}

Socket::~Socket(){
  if(fd!=INVALID_SOCKET){
    LogDEBUG<<"close socket "<<fd<<LogSend;
    ::close(fd);
  }
  fd=INVALID_SOCKET;
}

Socket Socket::accept() const{
  VERIFY_FD;
  SocketHandle _fd = ::accept(fd,0,0);
  THROW_WHEN(fd,INVALID_SOCKET,"accept error");
  return Socket(_fd);
}

int Socket::setsockopt(int name, int value) const {
  VERIFY_FD;
  return ::setsockopt(fd, SOL_SOCKET, name, &value, sizeof(value));
}

int Socket::bind(const NetAddress & addr) const{
  VERIFY_FD;
  if(setsockopt(SO_REUSEADDR, 1)==-1){
    return -1;
  }
  int ret=::bind(fd,addr.sockaddr_raw_ptr(), addr.addr_size());
  THROW_WHEN(ret,-1,"bind error");
  return ret;
}

int Socket::listen(int backlog_) const{
  VERIFY_FD;
  int ret= ::listen(fd, backlog_);
  THROW_WHEN(ret,-1,"listen error");
  return ret;

}

int Socket::connect(const NetAddress & addr) const {
  VERIFY_FD;
  int ret = ::connect(fd, addr.sockaddr_raw_ptr(), addr.addr_size());
  THROW_WHEN(ret,-1,"connect failed");
  return ret;
}

int Socket::setBlocking() const {
  VERIFY_FD;
  return ::ioctl(fd,FIONBIO,0);
}

int Socket::setNonBlocking() const {
  VERIFY_FD;
  return ::ioctl(fd,FIONBIO,1);
}


ssize_t Socket::write(const void * buf, size_t len) const {
  return ::write(fd,buf, len);
}

ssize_t Socket::read(void * buf_, size_t len) const{
  return ::read(fd, buf_, len);
}

