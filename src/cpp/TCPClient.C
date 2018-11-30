#include "TCPClient.H"
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

const int BufferSize = 1024;

void TCPClientConnection::connect(const NetAddress & _address){
  address = _address;
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd == -1){
    throw NetworkException("can't set up socket file discription");
  }
  if(::connect(sockfd, address.sockaddr_raw_ptr(),address.addr_size())){
    throw NetworkException("can't connect to");
  }
}


thread_local char recvbuf[BufferSize];

bool TCPClientConnection::send(const std::string & msg){
  // +1 for the /0
  if(::send(sockfd, msg.c_str(), msg.size()+1, 0)==-1){
     return false;
  }
  memset(recvbuf, 0 , BufferSize);
  ::recv(sockfd, recvbuf, sizeof(recvbuf), 0);
  std::string result(recvbuf);
  if(_callback) _callback("TCPClient", result);
  return true;
}


void TCPClientConnection::setCallBack(const CallBackFunc & cb){
  _callback = cb;
}

TCPClientConnection::~TCPClientConnection(){
  ::close(sockfd);
}


