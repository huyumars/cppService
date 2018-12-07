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
#include "NetLoop.H"
#include "Socket.H"
#include "NetChannel.H"

class TinyTCPServer {
  public:
    TinyTCPServer(const std::string host, int port):
      _socket(Socket::SocketType::TCP),
      _addr(host,port){
      }; 

    void run(){
      _netloopPtr = std::make_shared<NetLoop>();
      listen();
      _netloopPtr->start();
      /*
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
         */
    }

  private:
    void recv(const Socket &client){
      static char buffer[1024];
      memset(buffer, 0 ,sizeof(buffer));
      int len = client.recv(buffer, sizeof(buffer));
      if(len>0){
        clientData[client.handler()]=std::string(buffer);
      }
    }
    void notifySend(const Socket& client){
      //start monitor whether can write
      wchannels[client.handler()]->enable();
    }
    void send(const Socket & client){
      std::string & str = clientData[client.handler()];
      client.send(str.c_str(), str.size()+1);
      //finish send
      wchannels[client.handler()]->disable();
    }
    void _close(Socket::SocketHandle fd){
       rchannels.erase(fd);
       wchannels.erase(fd);
       echannels.erase(fd);
       clients.erase(fd);
       clientData.erase(fd);
       if(clients.size()==0){
         _netloopPtr->stop(); 
       }
    }
    void connect(const Socket &client){
      if(!client.valid()) return;
      client.setNonBlocking();
      Socket::SocketHandle fd = client.handler();
      rchannels[fd]= NetChannelFactory(_netloopPtr,
          client.handler(),
          NetChannel::Type::Read,
          "clientr",
          [this,&client](const NetChannel& nc){
          recv(client);
          notifySend(client);
          });
      rchannels[fd]->enable();
      wchannels[fd]= NetChannelFactory(_netloopPtr,
          client.handler(),
          NetChannel::Type::Write,
          "clientw",
          [this,&client](const NetChannel& nc){
          send(client);
          //if not enable won't trigger
          });
      echannels[fd]= NetChannelFactory(_netloopPtr,
          client.handler(),
          NetChannel::Type::Error,
          "cliente",
          [this](const NetChannel& nc){
          //if not enable won't trigger
          std::cout<<"close "<<nc.fd()<<std::endl;
          _close(nc.fd());
          });
      echannels[fd]->enable();
    }
    void listen(){
      _socket.bind(_addr);
      _socket.listen();
      _socket.setNonBlocking();
      listenChannel = NetChannelFactory(_netloopPtr,
          _socket.handler(),
          NetChannel::Type::Read,
          "tinyserverListener",
          [this](const NetChannel& nc){
          std::cout<<"get connect from"<<std::endl;
          auto clientSocket = _socket.accept();
          auto fd = clientSocket.handler();
          clients[fd]=std::move(clientSocket);
          connect(clients[fd]);
          });
      listenChannel->enable();
    }
    Socket     _socket;
    NetAddress _addr;
    NetLoop::Ptr   _netloopPtr;
    NetChannel::Ptr   listenChannel;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> rchannels;
    std::unordered_map<Socket::SocketHandle, std::string>     clientData;
    std::unordered_map<Socket::SocketHandle, Socket>          clients;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> wchannels;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> echannels;
};



#endif
