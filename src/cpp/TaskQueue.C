#include "TaskQueue.H"


void TaskQueue::apply(){
   while(!_impl.empty()){
     (_impl.front())();
     _impl.pop();
   }
}
