
#ifndef LOGTEST
#define LOGTEST

#include <gtest/gtest.h>

#include "Logger.H"


class LoggerTest:public ::testing::Test{};



TEST_F(LoggerTest,  infoTest){
  LOGINFO("hello world");
}


#endif
