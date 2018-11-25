#include <gtest/gtest.h>
#include "ThreadPool.H"

class TestCopyAndMoved {
  public:
    int i;
    TestCopyAndMoved(int _i):i(_i){};
    TestCopyAndMoved(const TestCopyAndMoved& a):i(a.i){
      std::cout<<"copied"<<std::endl;
    }
    TestCopyAndMoved( TestCopyAndMoved&& a):i(std::move(a.i)){
      std::cout<<"moved"<<std::endl;
    }

};


void f(TestCopyAndMoved a ){
  std::cout<<a.i<<std::endl;
}

TEST(testCase, test0)
{
  Executor e;
  std::future<void> res;
  for(int i=0; i< 10; ++i){
    res = e.task(f,TestCopyAndMoved(i));
  }
  res.wait();
}

int main(int argc, char **argv)
{
 testing::InitGoogleTest(&argc, argv);
 return RUN_ALL_TESTS();
}

