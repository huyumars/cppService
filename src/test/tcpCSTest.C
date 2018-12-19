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
        ::StrCallback scb;
        ::testing::Sequence seq;
        for(int i=1;i<10;++i){
          std::string result = "hello world "+std::to_string(i)+"from client"+std::to_string(thread_)+"|"+std::to_string(loop_);
          EXPECT_CALL(scb,val(::testing::StrEq(result))).InSequence(seq);
        }
        TCPClientConnection client;
        client.connect(NetAddress("127.0.0.1", 8888));
        client.setCallBack([thread_,loop_, &scb](std::string c, std::string msg){
            LogINFO<<thread_<<loop_<<"client:"<<"get msg from server "<<msg<<LogSend;
            scb.val(msg);
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
  ::IntCallback tcb;
  EXPECT_CALL(tcb,val(::testing::Eq(1)));
  EXPECT_CALL(tcb,val(::testing::Eq(2)));
  EXPECT_CALL(tcb,val(::testing::Eq(3)));
  server->wakeUp([&tcb](const NetLoop &){
      LogINFO<<"wakeup trigger 1"<<LogSend;
      tcb.val(1);
  });
  server->wakeUp([&tcb](const NetLoop &){
      LogINFO<<"wakeup trigger 2"<<LogSend;
      tcb.val(2);
  });
  server->wakeUp([&tcb](const NetLoop &){
      LogINFO<<"wakeup trigger 3"<<LogSend;
      tcb.val(3);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}


TEST_F(TcpCSTest, NetLoopTimerTest){
  ::IntCallback tcb;
  ::testing::Sequence  seq;
  EXPECT_CALL(tcb,val(::testing::Eq(1))).InSequence(seq);
  EXPECT_CALL(tcb,val(::testing::Eq(2))).InSequence(seq);
  server->_netloopPtr->addTimerT<Timer>("t1",std::chrono::milliseconds(100),[&tcb](const Timer &){
      LogINFO<<"timer trigger for 100ms"<<LogSend;
      tcb.val(2);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  server->_netloopPtr->addTimerT<Timer>("t2",std::chrono::milliseconds(50),[&tcb](const Timer &){
      LogINFO<<"timer trigger for 50ms"<<LogSend;
      tcb.val(1);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST_F(TcpCSTest, NetLoopServerCloseTest){
  server->asyncStop(); 
  serverrunner.join();
  //never left static object
  //which might call logger function
  server.reset();
}



