#ifndef TINYTCPSERVER
#define TINYTCPSERVER

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include <cstring>


#include "NetUtil.H"
#include "Socket.H"

class TinyTCPServer {
  public:
    TinyTCPServer(const std::string host, int port):
      _socket(Socket::SocketType::TCP),
      _addr(host,port){
      };
    

    void run(){
      _socket.bind(_addr);
      _socket.listen();
      Socket conn= _socket.accept();
      char buffer[1024];
      int i =0;
      while(1) {
        i++;
        memset(buffer, 0 ,sizeof(buffer));
        int len = conn.recv(buffer, sizeof(buffer));
        if(len<=0) break;
        std::string istr = std::to_string(i);
        strcpy(buffer+len-1, istr.c_str());
        conn.send(buffer,len+istr.size()+1);
      }
    }

  private:
    Socket     _socket;
    NetAddress _addr;
};



#endif
