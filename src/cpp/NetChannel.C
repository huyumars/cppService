#include "NetChannel.H"
#include <sys/eventfd.h>

NetChannel::NetChannel(
    const NetLoop::Ptr & nlp,
    Socket::SocketHandle fd,
    Type type,
    const std::string& name,
    const FunctorType &cb):
    _netloopPtr(nlp),
    _fd(fd),
    _type(type),
    _name(name),
    _cbfunctor(cb){
}

void NetChannel::process(){
  if(_cbfunctor) _cbfunctor(*this);
}


void NetChannel::enable() {
  if(_netloopPtr.expired()) return;
  _netloopPtr.lock()->addChannel(*this);
}


void NetChannel::disable(){
  if(_netloopPtr.expired()) return;
  _netloopPtr.lock()->rmChannel(*this);
}

NetChannel::~NetChannel(){
  disable();
}


//wake up channel
WakeUpChannel::~WakeUpChannel(){}

WakeUpChannel::WakeUpChannel(const NetLoop::Ptr & netloopPtr, const FunctorType & cb):
  NetChannel(netloopPtr, ::eventfd(0,0),Type::Read,"wakeupchannel",cb),
  _eventSocket(fd())
{ }


void WakeUpChannel::notify() const {
  uint64_t u = 1;
  auto r = _eventSocket.write(&u,sizeof(uint64_t));
  if(r!=sizeof(uint64_t)) throw NetException("notify send failed");
  //don't close the fd by swap
}

void WakeUpChannel::ack() const {
  uint64_t u;
  auto r = _eventSocket.read(&u,sizeof(uint64_t));
  if(r!=sizeof(uint64_t)) throw NetException("ack recv failed");
}

