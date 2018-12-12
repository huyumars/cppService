#ifndef TCPCSTEST
#define TCPCSTEST

#include <gtest/gtest.h>
#include "TCPClient.H"
#include "tinyTCPServer.H"
#include "testUtility.H"
#include <thread>
#include "Logger.H"

class TcpCSTest: public ::testing::Test{
  public:
  static void SetUpTestCase() {
    server = std::make_shared<TinyTCPServer>("127.0.0.1", 8888);
    serverrunner =std::thread(
      [](){
      server->run();
      LogINFO<<"server will close now "<<LogSend;
    });
  }
  static std::shared_ptr<TinyTCPServer> server;
  static std::thread                    serverrunner;
};

std::shared_ptr<TinyTCPServer> TcpCSTest::server=std::shared_ptr<TinyTCPServer>();
std::thread                    TcpCSTest::serverrunner=std::thread();


TEST_F(TcpCSTest, TCPClientConnectionTest){
  Logger::addDest("stdout",LogDest::Type::stdout); 
  //wait 1s to let server start;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ::MutiltiThreadTest(1,1,[](int thread_, int loop_)
      {
      try{
        TCPClientConnection client;
        client.connect(NetAddress("127.0.0.1", 8888));
        client.setCallBack([thread_,loop_](std::string c, std::string msg){
            LogINFO<<thread_<<loop_<<"client:"<<"get msg from server "<<msg<<LogSend;
            });
        for(int i=1;i<10;++i){
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        if(!client.send("hello world "+std::to_string(i)+"from client"+std::to_string(thread_)+"|"+std::to_string(loop_)))
           throw ::NetException("sent failed");
        }
      }
      catch(std::exception & ex){
      std::cerr<<"Exception"<<ex.what()<<std::endl;
      }

   });
  LogINFO<<"server will close now "<<LogSend;
}

TEST_F(TcpCSTest, NetLoopServerCloseTest){
  server->asyncStop(); 
  serverrunner.join();
  //never left static object
  //which might call logger function
  server.reset();
}

#endif
