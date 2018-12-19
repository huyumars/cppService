#ifndef TCPCSTEST
#define TCPCSTEST

#include <gtest/gtest.h>
#include <thread>

void _setupTestcase();

class TinyTCPServer;
class TcpCSTest: public ::testing::Test{
  public:
  static void SetUpTestCase(){
    _setupTestcase();
  };
  static std::shared_ptr<TinyTCPServer> server;
  static std::thread                    serverrunner;
};



#endif
