#include<iostream>
#include "ThreadPool.H"


void f(int i ){
  std::cout<<i<<std::endl;
}

int main(){
  Executor e;
  for(int i=0; i< 100; ++i){
    e.task([]{std::cout<<"jjj"<<std::endl;});
  }
  e.join();
  return 0;
}
