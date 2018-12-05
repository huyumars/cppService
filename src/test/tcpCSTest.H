#ifndef TCPCSTEST
#define TCPCSTEST

#include <gtest/gtest.h>
#include "TCPClient.H"
#include "testUtility.H"
#include <thread>

class TcpCSTest: public ::testing::Test{};



TEST_F(TcpCSTest, TCPClientConnectionTest){
  TCPClientConnection client;
  try{

    client.connect(NetAddress("127.0.0.1", 8888));
    client.setCallBack([](std::string c, std::string msg){
        std::cout<<"get msg from server"<<msg<<std::endl;
    });
    int i =0;
    while(1){
      i++;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
       client.send("hello world "+std::to_string(i));
    }
    /*
    MutiltiThreadTest(10,10,[&client](int thread_id, int loop_id){
        client.send("send from "+std::to_string(thread_id)+" in loop " + std::to_string(loop_id));
    });
    */
  }
  catch(std::exception & ex){
    std::cerr<<"Exception"<<ex.what()<<std::endl;
  }
}

#endif
