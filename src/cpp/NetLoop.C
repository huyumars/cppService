#include "NetLoop.H"
#include "NetChannel.H"

void NetLoop::runLoop(){
  for(;;){
    ioMulitplex(0);
  }
}


void NetLoop::ioMulitplex(int tiemout){
  
}
