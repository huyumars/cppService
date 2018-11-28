#ifndef TESTUTILITY
#define TESTUTILITY

#include <functional>
#include <vector>
#include <thread>

void MutiltiThreadTest(int threadNum, int loopNumber, std::function<void(int,int)> f){
  std::vector<std::thread> threads;
  for(int i =0; i< 20; ++i){
    threads.push_back(std::thread([i,f](){
      for(int j =0; j< 10; ++j){
         f(i,j);
       }
    }));
  }
  for(std::thread & t : threads){
    t.join();
  }

}

#endif
