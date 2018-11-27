
#ifndef LOGTEST
#define LOGTEST

#include <gtest/gtest.h>

#include "Logger.H"
#include <sstream>


class LoggerTest:public ::testing::Test{};



TEST_F(LoggerTest,  infoTest){
  Singleton<Logger>::instance().addDest(LogDest(LogDest::Type::stdout));
  LOGINFO("hello world");
}

TEST_F(LoggerTest,  mutiThreadTest){
  std::vector<std::thread> threads;
  for(int i =0; i< 20; ++i){
    threads.push_back(std::thread([i](){
      for(int j =0; j< 10; ++j){
          std::stringstream ss;
          ss<<"hello world from"<<i<<"thread"<<j<<"times";
          LOGINFO(ss.str());
       }
    }));
  }
  for(std::thread & t : threads){
    t.join();
  }
}


#endif
