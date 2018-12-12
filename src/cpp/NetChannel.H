#ifndef NETCHANNEL
#define NETCHANNEL

#include <memory>
#include <functional>
#include "NetLoop.H"
#include "Socket.H"
#include "Types.H"




class NetChannel:public SharedObject<NetChannel>{
  public:
    enum class Type:int{
      Read,
      Write,
      Error,
    };
    typedef std::function<void(const NetChannel&)> FunctorType;
    typedef std::unique_ptr<NetChannel>  UPtr;

    virtual ~NetChannel();
    void enable();
    void disable();
    //WPtr weak_pointer() const {return _weakptr;}
    Socket::SocketHandle fd() const {return _fd;}
    Type type() const {return _type;}
    std::string name() const {return _name;}
    FunctorType callback() const {return _cbfunctor;}

    virtual void process();
    NetChannel(const NetLoop::Ptr &netloopPtr,
               Socket::SocketHandle,
               Type type,
               const std::string& name,
               const FunctorType &);
  private:
    NetLoop::WPtr          _netloopPtr;
    Socket::SocketHandle   _fd;
    Type                   _type;
    std::string            _name;
    FunctorType            _cbfunctor;
    WPtr                   _weakptr;
};

class WakeUpChannel: public NetChannel {
  public:
    typedef std::shared_ptr<WakeUpChannel> Ptr;
    typedef std::weak_ptr<WakeUpChannel>  WPtr;
    typedef SharedFactory<WakeUpChannel>  Factory;
    ~WakeUpChannel() override;
    WakeUpChannel(const NetLoop::Ptr &netloopPtr,
                  const FunctorType &);
    void notify() const;
    void ack() const;
  private:
    // only for event fd;
    friend  Factory;
    Socket _eventSocket;
};


#endif
