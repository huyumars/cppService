#include "NetUtil.H"

#include <arpa/inet.h>
#include <cstring>


NetAddress::NetAddress(const std::string & host, unsigned int port):
  NetAddress(){
  _data.sin_family = AF_INET;
  _data.sin_addr.s_addr = inet_addr(host.c_str());
  _data.sin_port = htons(port);
}

NetAddress::NetAddress(){
  memset(&_data, 0, sizeof(_data));
}
