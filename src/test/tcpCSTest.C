#include "tcpCSTest.H"
#include "TCPClient.H"
#include "tinyTCPServer.H"
#include "testUtility.H"
#include "Logger.H"
#include "Timer.H"


std::shared_ptr<TinyTCPServer> TcpCSTest::server=std::shared_ptr<TinyTCPServer>();
std::thread                    TcpCSTest::serverrunner=std::thread();



void _setupTestcase() {
  TcpCSTest::server = std::make_shared<TinyTCPServer>("127.0.0.1", 8888);
  TcpCSTest::serverrunner =std::thread(
    [](){
    TcpCSTest::server->run();
    LogINFO<<"server will close now "<<LogSend;
  });
}


TEST_F(TcpCSTest, TCPClientConnectionTest){
  //Logger::addDest("stdout",LogDest::Type::stdout); 
  //wait 1s to let server start;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ::MutiltiThreadTest(10,3,[](int thread_, int loop_)
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

TEST_F(TcpCSTest, NetLoopWakeUpTest){
  server->wakeUp([](const NetLoop &){
      LogCRITICAL<<"wakeup trigger 1"<<LogSend;
  });
  server->wakeUp([](const NetLoop &){
      LogCRITICAL<<"wakeup trigger 2"<<LogSend;
  });
  server->wakeUp([](const NetLoop &){
      LogCRITICAL<<"wakeup trigger 3"<<LogSend;
  });
  std::this_thread::sleep_for(std::chrono::seconds(1));
}


TEST_F(TcpCSTest, NetLoopTimerTest){
  server->_netloopPtr->addTimerT<Timer>("t1",std::chrono::seconds(2),[](const Timer &){
      LogCRITICAL<<"timer trigger for 2s"<<LogSend;
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  server->_netloopPtr->addTimerT<Timer>("t2",std::chrono::seconds(1),[](const Timer &){
      LogCRITICAL<<"timer trigger for 1s"<<LogSend;
  });
  std::this_thread::sleep_for(std::chrono::seconds(3));
}

TEST_F(TcpCSTest, NetLoopServerCloseTest){
  server->asyncStop(); 
  serverrunner.join();
  //never left static object
  //which might call logger function
  server.reset();
}



