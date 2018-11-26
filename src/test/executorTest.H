#ifndef EXECUTORTEST
#define EXECUTORTEST

#include <gtest/gtest.h>
#include "Executor.H"
#include <vector>

class ExecutorTest: public ::testing::Test{
};


TEST_F(ExecutorTest, safeCloseTest){
  for(int i=0; i<100;++i){
    Executor e;
    e.task([]{});
  }
}


class TestCopyAndMoved {
  public:
    int i;
    static int copied_times;
    static int moved_times;
    TestCopyAndMoved(int _i):i(_i){};
    TestCopyAndMoved(const TestCopyAndMoved& a):i(a.i){
      copied_times++;
    }
    TestCopyAndMoved( TestCopyAndMoved&& a):i(std::move(a.i)){
      moved_times++;
    }

};
int TestCopyAndMoved::copied_times = 0;
int TestCopyAndMoved::moved_times = 0;

template <typename T>
T identity(T a ){
  return a;
}

void _returnvalueTest(){
  Executor e;
  std::vector<std::future<int>> res;
  for(int i=0; i< 10; ++i){
    res.push_back(e.task(identity<int>,i));
  }
  int i =0;
  for(std::future<int> &r : res){
    r.wait();
    ASSERT_EQ(r.get(),i);
    i++;
  }
}


TEST_F(ExecutorTest, returnValueTest)
{
  for(int i = 0; i<20; ++i){
    _returnvalueTest();
  }
}

TEST_F(ExecutorTest, copyMoveTimesTest){
  TestCopyAndMoved a(99);
  TestCopyAndMoved::copied_times = 0;
  TestCopyAndMoved::moved_times = 0;
  Executor e;
  auto res = e.task(identity<TestCopyAndMoved>, std::move(a));
  res.wait();
  EXPECT_EQ(TestCopyAndMoved::moved_times,4); // std::move, 2 for convert inside, one for return 2 for convert inside, one for return 2 for convert inside, one for return 
  EXPECT_EQ(TestCopyAndMoved::copied_times,1); // one for store argurment
}

#endif
