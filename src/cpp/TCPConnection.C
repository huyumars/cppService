#include "TCPConnection.H"
#include "Logger.H"

TCPConnection::TCPConnection(NetLoop::Ptr netloop)
  :_netloop(netloop){
}

void TCPConnection::readHandle(const NetChannel & nc){
  static char buffer[1024];
  memset(buffer, 0 ,sizeof(buffer));
  int len = _socket.recv(buffer, sizeof(buffer));
  if(len>0){
    LogINFO<<"recv from client"<<_socket.handler()<<LogSend;
    if(_rcb) _rcb(std::string(buffer));
  }
  else{
    if(_ecb) _ecb(*this);
  }
}
void TCPConnection::writeHandle(const NetChannel & nc){
  PCQueue::QueuePtr   datas;
  _writeQueue.tryGet(datas);
  while(datas->size()){
    Data & data = datas->front();
    _socket.send(data.c_str(), data.size()+1);
    datas->pop();
  }
  _wchannel->disable();
}

void TCPConnection::asyncSend(const Data &data){
  _writeQueue.put(data);
  //notify to send
  _wchannel->enable();
}

void TCPConnection::errorHandle(const NetChannel & nc){
  if(_ecb) _ecb(*this);
}

void TCPConnection::connect(Socket && client){
  _socket = std::move(client);
  NetLoop::Ptr netloop = _netloop.lock();
  if(!netloop||!_socket.valid()) return;
  _socket.setNonBlocking();
  _rchannel= NetChannel::Factory::create(netloop,
      _socket.handler(),
      NetChannel::Type::Read,
      "clientr",
      [this](const NetChannel& nc){
         readHandle(nc);
      });
  _rchannel->enable();
  _wchannel= NetChannel::Factory::create(netloop,
      _socket.handler(),
      NetChannel::Type::Write,
      "clientw",
      [this](const NetChannel& nc){
         writeHandle(nc);
      //if not enable won't trigger
      });
  _echannel= NetChannel::Factory::create(netloop,
     _socket.handler(),
     NetChannel::Type::Error,
     "cliente",
     [this](const NetChannel& nc){
     //if not enable won't trigger
     LogINFO<<"get error"<<LogSend;
       errorHandle(nc);
     });
  _echannel->enable();
}
