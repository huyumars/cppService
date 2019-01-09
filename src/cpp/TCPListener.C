#include "TCPListener.H"
#include "NetChannel.H"
#include "Logger.H"


TCPListener::TCPListener(NetLoop::Ptr netloop, const NetAddress & addr)
  :_state(State::invalid),_netloop(netloop),_addr(addr)
{}


bool TCPListener::listen(){
  _socket = Socket(Socket::SocketType::TCP);
  auto netloop = _netloop.lock();
  if(_socket.valid()&&netloop){
    _socket.bind(_addr);
    _socket.listen();
    _socket.setNonBlocking();
    _state = State::listen;
    _listenChannel = NetChannel::Factory::create(netloop,
          _socket.handler(),
          NetChannel::Type::Read,
          "listenerChannel",
          [this](const NetChannel& nc){
          acceptHandle(nc);
    });
    _listenChannel->enable();
    return true;;
  }
  else{
    _state = State::error;
    return false;
  }
}

void TCPListener::acceptHandle(const NetChannel & nc){
  auto clientSocket = _socket.accept();
  LogINFO<<"accept connect "<<clientSocket.handler()<<LogSend;
  _acb(std::move(clientSocket));
}


