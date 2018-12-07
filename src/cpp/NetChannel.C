#include "NetChannel.H"

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
  _weakptr = shared_from_this();
  _netloopPtr.lock()->addChannel(_weakptr);
}


void NetChannel::disable(){
  _netloopPtr.lock()->rmChannel(_weakptr);
}

NetChannel::~NetChannel(){
  disable();
}


NetChannel::Ptr NetChannelFactory(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle fd,
               NetChannel::Type type,
               const std::string& name,
               const NetChannel::FunctorType &functor){
  auto ncp = std::shared_ptr<NetChannel>(new NetChannel(netloopPtr, fd, type, name, functor));
  return ncp;
}


