#ifndef THREADSAFETEST
#define THREADSAFETEST

#include <gtest/gtest.h>
#include "ThreadSafe.H"
#include "testUtility.H"
#include <set>
#include <mutex>

class SingletonTest: public ::testing::Test{
  public:
    class DeleteObject{
      typedef std::function<void(DeleteObject *)>  Observer;
      public:
        DeleteObject()=default;
        void set(Observer obs){_obs=obs;}
        ~DeleteObject(){
          if(_obs) _obs(this);
        }
      private:
        Observer _obs;
    };
};


TEST_F(SingletonTest, singletonTest){
  typedef typename SingletonTest::DeleteObject A;
  threads::Queue<A*> q;
  MutiltiThreadTest(20,1,[&q](int i, int j){
     q.push(&threads::Singleton<A>::instance());
  });
  A * address = nullptr;
  while(!q.empty()){
    if(address == nullptr) address = q.front();
    else EXPECT_EQ(address, q.front());
    q.pop();
  }
}


TEST_F(SingletonTest, threadSingletonTest){
  typedef typename SingletonTest::DeleteObject A;
  std::mutex m;
  std::set<A*> alivePointers;
  auto observer = [&alivePointers,&m](A *a){
    std::unique_lock<std::mutex> lock(m);
    alivePointers.erase(a);
  };
  MutiltiThreadTest(20,1,[&alivePointers,&m,observer](int i, int j){
      A & a = threads::Singleton<threads::ThreadLocal<A>>::instance();
      {
        std::unique_lock<std::mutex> lock(m);
        alivePointers.insert(&a);
      }
      a.set(observer);
  });
  EXPECT_EQ(alivePointers.size(),0);

}


#endif


