#ifndef TINYTCPSERVER
#define TINYTCPSERVER


#include <condition_variable>
#include <mutex>
#include "NetUtil.H"

#include "NetLoop.H"
#include "NetLoopThread.H"
#include "TCPListener.H"
#include "TCPConnection.H"
#include "Socket.H"
#include "NetChannel.H"
#include "Logger.H"

class TinyTCPServer {
  public:
    TinyTCPServer(const std::string host, int port):
      _addr(host,port),_curid(0){
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
    void recvingMessage(int netid, const TCPConnection::Data & data){
      connections[netid]->asyncSend(data); 
    }
    void listen(){
      listener->setAcceptCallBack(
          [this](Socket&& clientSocket){
            _curid ++;
            int netid = _curid;
            auto connection = std::make_shared<TCPConnection>(netloop());
            connection->connect(std::forward<Socket>(clientSocket));
            connections[netid] = connection;
            connection->setReadCallBack([this, netid](const TCPConnection::Data & data){
                recvingMessage(netid, data);
             });
            connection->setErrorCallBack([this,netid](TCPConnection &){
                connections.erase(netid);
             });
          });
      listener->listen();
    }
    NetAddress _addr;
    NetLoopThread     netloopthread;
    std::shared_ptr<TCPListener>       listener;
    int _curid;
    std::unordered_map<int,std::shared_ptr<TCPConnection>>     connections;
};



#endif
