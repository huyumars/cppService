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
  _cbfunctor(*this);
}

NetChannel::~NetChannel(){
}
