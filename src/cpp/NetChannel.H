#ifndef NETCHANNEL
#define NETCHANNEL

#include <memory>
#include <functional>
#include "NetLoop.H"
#include "Socket.H"

class NetChannel : public std::enable_shared_from_this<NetChannel>{
  public:
    enum class Type:int{
      Read,
      Write,
      Error,
    };
    typedef std::function<void(const NetChannel&)> FunctorType;
    typedef std::shared_ptr<NetChannel>  Ptr;
    typedef std::shared_ptr<NetChannel>  WPtr;
    typedef std::unique_ptr<NetChannel>  UPtr;

    ~NetChannel();
    void enable();
    void disable();
    Socket::SocketHandle fd() const {return _fd;}
    Type type() const {return _type;}
    std::string name() const {return _name;}
    FunctorType callback() const {return _cbfunctor;}

    virtual void process();
    friend Ptr NetChannelFactory(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle,
               Type type,
               const std::string& name,
               const FunctorType &);
  private:
    //private constructor , using factory to create oject on heap with std::ptr
    NetChannel(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle,
               Type type,
               const std::string& name,
               const FunctorType &);
    NetLoop::WPtr          _netloopPtr;
    Socket::SocketHandle   _fd;
    Type                   _type;
    std::string            _name;
    FunctorType            _cbfunctor;
    WPtr                   _weakptr;
};


NetChannel::Ptr NetChannelFactory(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle,
               NetChannel::Type type,
               const std::string& name,
               const NetChannel::FunctorType &);

#endif
