#include "Socket.H"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

Socket::Socket():fd(Socket::INVALID_SOCKET){}

Socket::Socket(SocketHandle handle){
  ::close(fd);
  fd = handle;
}



