#ifndef TINYTCPSERVER
#define TINYTCPSERVER


#include "NetUtil.H"
#include "NetLoop.H"
#include "Socket.H"
#include "NetChannel.H"
#include "Logger.H"

class TinyTCPServer {
  public:
    TinyTCPServer(const std::string host, int port):
      _socket(Socket::SocketType::TCP),
      _addr(host,port){
      }; 

    void run(){
      try{
      _netloopPtr = NetLoop::Factory::create();
      listen();
      _netloopPtr->start();
      }
      catch(std::exception& ex){
         LogERROR<<ex.what()<<LogSend;
      }
    }
    void asyncStop(){
      try{
      _netloopPtr->asyncStop();
      }
      catch(std::exception&ex){
         LogERROR<<ex.what()<<LogSend;
      }
    }
    void wakeUp(const NetLoop::WakeUpCB &cb){
      try{
      _netloopPtr->asyncWakeUp(cb);
      }
      catch(std::exception&ex){
         LogERROR<<ex.what()<<LogSend;
      }
    }

 //private:
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
      rchannels[fd]= NetChannel::Factory::create(_netloopPtr,
          client.handler(),
          NetChannel::Type::Read,
          "clientr",
          [this,&client](const NetChannel& nc){
          recv(client);
          });
      rchannels[fd]->enable();
      wchannels[fd]= NetChannel::Factory::create(_netloopPtr,
          client.handler(),
          NetChannel::Type::Write,
          "clientw",
          [this,&client](const NetChannel& nc){
          send(client);
          //if not enable won't trigger
          });
      echannels[fd]= NetChannel::Factory::create(_netloopPtr,
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
      _socket.bind(_addr);
      _socket.listen();
      _socket.setNonBlocking();
      listenChannel = NetChannel::Factory::create(_netloopPtr,
          _socket.handler(),
          NetChannel::Type::Read,
          "tinyserverListener",
          [this](const NetChannel& nc){
          auto clientSocket = _socket.accept();
          auto fd = clientSocket.handler();
          LogINFO<<"get connection , and open"<<fd<<LogSend;
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
