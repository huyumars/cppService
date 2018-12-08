#include<iostream>
#include "Logger.H"

int main(){
  Logger::addDest("stdout",LogDest::Type::stdout);
  std::vector<std::thread> threads;
  for (int i = 0; i < 20; ++i) {
  threads.push_back(std::thread([i]() {
    for (int j = 0; j < 10; ++j) {
      LogINFO << "JJBB  " <<i<<" "<<j<<" "<< LogSend;
     }
   }));
  }
  for (std::thread & t : threads) {
    t.join();
  }
}
