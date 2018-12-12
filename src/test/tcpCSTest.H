#ifndef TCPCSTEST
#define TCPCSTEST

#include <gtest/gtest.h>
#include "TCPClient.H"
#include "tinyTCPServer.H"
#include "testUtility.H"
#include <thread>
#include "Logger.H"

class TcpCSTest: public ::testing::Test{};



TEST_F(TcpCSTest, TCPClientConnectionTest){
  Logger::addDest("stdout",LogDest::Type::stdout); 
  std::thread serverrunner(
      [](){
      TinyTCPServer server("127.0.0.1",8888);
      server.run();
      });
  //wait 1s to let server start;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ::MutiltiThreadTest(3,3,[](int thread_, int loop_)
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
  /*
     MutiltiThreadTest(10,10,[&client](int thread_id, int loop_id){
     client.send("send from "+std::to_string(thread_id)+" in loop " + std::to_string(loop_id));
     });
     */
  serverrunner.join();
}

#endif
