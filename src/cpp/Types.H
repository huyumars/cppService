#ifndef _TYPES
#define _TYPES

#include <memory>

template<typename T>
class SharedFactory {
  public:
  template<typename ...Args>
  static std::shared_ptr<T> create(Args && ...args){
    auto sp = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    //set weak ptr for self;
    sp->set_self_weak_ptr(sp);
    return sp;
  };
};


template<typename T>
class SharedObject {
  public:
    typedef SharedFactory<T> Factory;
    typedef std::shared_ptr<T>   Ptr;
    typedef std::weak_ptr<T>    WPtr;
    friend Factory;
    WPtr get_weak() const {return _weakptr;}
    void set_self_weak_ptr(const Ptr & sp){_weakptr = sp;}
  protected:
    SharedObject()=default;
  private:
    WPtr    _weakptr;
};

template<typename T>
class RawPointer {
  public:
    RawPointer(const T &t):_p(&t){}
    T &operator->() const {return *_p;}
  private:
    T *_p;
};

#endif

