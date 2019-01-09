#ifndef TINYTCPSERVER
#define TINYTCPSERVER


#include <condition_variable>
#include <mutex>
#include "NetUtil.H"

#include "NetLoop.H"
#include "NetLoopThread.H"
#include "TCPListener.H"
#include "Socket.H"
#include "NetChannel.H"
#include "Logger.H"

class TinyTCPServer {
  public:
    TinyTCPServer(const std::string host, int port):
      _addr(host,port){
      };

    void start(){
      netloopthread.start();
      listener = std::make_shared<TCPListener>(netloop(),_addr);
      listen();
    }
    void stop(){
      netloopthread.stop();
    }
    NetLoop::Ptr  netloop() const {return netloopthread.netloop();}
    ~TinyTCPServer(){
      netloopthread.join();
    }

 private:
    void recv(const Socket &client){
      static char buffer[1024];
      memset(buffer, 0 ,sizeof(buffer));
      int len = client.recv(buffer, sizeof(buffer));
      if(len>0){
        LogINFO<<"recv from client"<<client.handler()<<LogSend;
        clientData[client.handler()]=std::string(buffer);
        notifySend(client);
      }
      else{
        _close(client.handler());
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
      if(clients.count(fd)==0) return;
       rchannels.erase(fd);
       wchannels.erase(fd);
       echannels.erase(fd);
       clients.erase(fd);
       clientData.erase(fd);
    }
    void connect(const Socket &client){
      if(!client.valid()) return;
      client.setNonBlocking();
      Socket::SocketHandle fd = client.handler();
      rchannels[fd]= NetChannel::Factory::create(netloop(),
          client.handler(),
          NetChannel::Type::Read,
          "clientr",
          [this,&client](const NetChannel& nc){
          recv(client);
          });
      rchannels[fd]->enable();
      wchannels[fd]= NetChannel::Factory::create(netloop(),
          client.handler(),
          NetChannel::Type::Write,
          "clientw",
          [this,&client](const NetChannel& nc){
          send(client);
          //if not enable won't trigger
          });
      echannels[fd]= NetChannel::Factory::create(netloop(),
          client.handler(),
          NetChannel::Type::Error,
          "cliente",
          [this](const NetChannel& nc){
          //if not enable won't trigger
          LogINFO<<"get error"<<LogSend;
          _close(nc.fd());
          });
      echannels[fd]->enable();
    }
    void listen(){
      listener->setAcceptCallBack(
          [this](Socket&& clientSocket){
          auto fd = clientSocket.handler();
          clients[fd]=std::move(clientSocket);
          connect(clients[fd]);
          });
      listener->listen();
    }
    NetAddress _addr;
    NetLoopThread     netloopthread;
    std::shared_ptr<TCPListener>       listener;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> rchannels;
    std::unordered_map<Socket::SocketHandle, std::string>     clientData;
    std::unordered_map<Socket::SocketHandle, Socket>          clients;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> wchannels;
    std::unordered_map<Socket::SocketHandle, NetChannel::Ptr> echannels;
};



#endif
