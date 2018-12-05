#ifndef NETCHANNEL
#define NETCHANNEL

#include <memory>
#include <functional>
#include "NetLoop.H"
#include "Socket.H"

class NetChannel{
  public:
    enum class Type:int{
      Read,
      Write,
      Error,
    };
    typedef std::function<void(const NetChannel&)> FunctorType;
    typedef std::shared_ptr<NetChannel>  Ptr;
    typedef std::unique_ptr<NetChannel>  UPtr;

    NetChannel(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle,
               Type type,
               const std::string& name,
               const FunctorType &);
    ~NetChannel();

    Socket::SocketHandle fd() const {return _fd;}
    Type type() const {return _type;}
    std::string name() const {return _name;}
    FunctorType callback() const {return _cbfunctor;}

    virtual void process();

  
  private:
    NetLoop::WPtr          _netloopPtr;
    Socket::SocketHandle   _fd;
    Type                   _type;
    std::string            _name;
    FunctorType            _cbfunctor;

};

#endif
