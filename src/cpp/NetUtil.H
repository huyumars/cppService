#ifndef NETUTIL
#define NETUTIL

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <memory>

class NetAddress{
  public:
    NetAddress(const std::string & host, unsigned int port);
    NetAddress();
    sockaddr *sockaddr_raw_ptr() const {return (struct sockaddr *)(&_data); }
    static constexpr size_t addr_size() {return sizeof(sockaddr);}
  private:
    sockaddr_in _data;
};


class StringException:public std::exception{
  public:
    StringException(const std::string &msg_):_msg(msg_){}
    const char * what() const noexcept override{ return _msg.c_str();}
  protected:
    std::string _msg;
};


class NetException:public StringException {
  public:
    NetException(const std::string&msg_):StringException("[NetException]"+msg_){}
};

#endif
