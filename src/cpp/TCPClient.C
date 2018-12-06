#include "TCPClient.H"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

#include <iostream>

const int BufferSize = 1024;

TCPClientConnection::TCPClientConnection():
  socket(Socket::SocketType::TCP){}

TCPClientConnection::~TCPClientConnection() { }

void TCPClientConnection::connect(const NetAddress & _address){
  address = _address;
  socket.connect(address);
}


thread_local char recvbuf[BufferSize];

bool TCPClientConnection::send(const std::string & msg){
  // +1 for the /0
  if(socket.send( msg.c_str(), msg.size()+1)==-1){
     return false;
  }
  memset(recvbuf, 0 , BufferSize);
  socket.recv(recvbuf, sizeof(recvbuf));
  std::string result(recvbuf);
  if(_callback) _callback("TCPClient", result);
  return true;
}


void TCPClientConnection::setCallBack(const CallBackFunc & cb){
  _callback = cb;
}



