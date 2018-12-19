#ifndef _TESTUTILITY
#define _TESTUTILITY

#include <gmock/gmock.h>
#include <functional>
#include <vector>
#include <thread>

inline void MutiltiThreadTest(int threadNum, int loopNumber, std::function<void(int,int)> f){
  std::vector<std::thread> threads;
  for(int i =0; i< threadNum; ++i){
    threads.push_back(std::thread([i,f,loopNumber](){
      for(int j =0; j< loopNumber; ++j){
         f(i,j);
       }
    }));
  }
  for(std::thread & t : threads){
    t.join();
  }

}

template<typename T>
class CallBack{
  public:
    virtual ~CallBack(){};
    void virtual val(const T &x){ }
};

class IntCallback: public CallBack<int>{
  public:
   MOCK_METHOD1(val,void(const int &));
};

class StrCallback: public CallBack<std::string>{
  public:
   MOCK_METHOD1(val,void(const std::string &));
};

#endif
