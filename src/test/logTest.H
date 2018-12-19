
#ifndef LOGTEST
#define LOGTEST

#include <gtest/gtest.h>

#include "Logger.H"
#include "testUtility.H"
#include <sstream>


class LoggerTest:public ::testing::Test{};



TEST_F(LoggerTest,  infoTest){
  Logger::addDest("fileLog","/var/tmp/log.log");
  LOGINFO("hello world");
}



TEST_F(LoggerTest,  mutiThreadTest1){
  MutiltiThreadTest(10, 10, [](int i, int j){
    LogINFO<<"hello world from"<<i<<"thread"<<j<<"times"<<LogSend;
  });
}

TEST_F(LoggerTest,  mutiThreadTest2){
  MutiltiThreadTest(10, 10, [](int i, int j){
      LOGINFO("mutiThreadTest2 "+std::to_string(i)+" "+std::to_string(j));
  });
}

TEST_F(LoggerTest,  coutStyleinfoTest){
  int a = 100;
  float z = 10.9;
  LogINFO <<"cout style"<<a<<"  "<<z<<LogSend;
}


TEST_F(LoggerTest,  logLevelTest){
  LogINFO <<"INFO log"<<LogSend;
  Logger::addDest("err",LogDest::Type::stderr);
  Logger::getDest("err").set_log_level(LogType::error);
  LogDEBUG<<"DEBUG log"<<LogSend;
  LogINFO <<"INFO log"<<LogSend;
  LogERROR<<"ERROR log"<<LogSend;
  LogCRITICAL<<"CRITICAL log"<<LogSend;

}

#endif
