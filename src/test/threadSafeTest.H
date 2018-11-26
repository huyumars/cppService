#ifndef THREADSAFETEST
#define THREADSAFETEST

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <memory>
#include "ThreadSafe.H"


class SingletonTest: public ::testing::Test{
  public:
    class A{};
};


TEST_F(SingletonTest, singletonTest){
  typedef typename SingletonTest::A A;
  SafeQueue<A*> q;
  std::vector<std::shared_ptr<std::thread>> threads;
  for (int i=0; i<10; ++i){
    threads.push_back(std::make_shared<std::thread>([&q]{
          q.push(&Singleton<A>::instance());
     }));
  }
  for(auto ptr: threads){
    ptr->join();
  }
  A * address = nullptr;
  while(!q.empty()){
    if(address == nullptr) address = q.front();
    else EXPECT_EQ(address, q.front());
    q.pop();
  }
}


#endif


