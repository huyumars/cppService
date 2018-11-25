#include<iostream>
#include "ThreadPool.H"

class A {
  public:
    int i;
    A(int _i):i(_i){};
    A(const A& a):i(a.i){
      std::cout<<"copied"<<std::endl;
    }
    A( A&& a):i(std::move(a.i)){
      std::cout<<"moved"<<std::endl;
    }

};

void f(A a ){
  std::cout<<a.i<<std::endl;
}

int main(){
  Executor e;
  std::future<void> res;
  for(int i=0; i< 10; ++i){
    res = e.task(f,A(i));
  }
  res.wait();
  return 0;
}
