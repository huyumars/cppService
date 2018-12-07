#ifndef TESTUTILITY
#define TESTUTILITY

#include <functional>
#include <vector>
#include <thread>

void MutiltiThreadTest(int threadNum, int loopNumber, std::function<void(int,int)> f){
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

#endif
