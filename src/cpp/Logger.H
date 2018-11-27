#ifndef LOGGER
#define LOGGER

#include "ThreadSafe.H"
#include <iostream>
#include <thread>
#include <unordered_map>
#include <atomic>

#define LOGFUNC(_str_, _level_)  \
    LogMsg::Log(_str_,__LINE__ , __FILE__,_level_)


#define LOGINFO(_str_)  LOGFUNC(_str_, LogType::info)
#define LOGDEBUG(_str_)  LOGFUNC(_str_, LogType::debug)
#define LOGERROR(_str_)  LOGFUNC(_str_, LogType::error)
#define LOGCRITICAL(_str_)  LOGFUNC(_str_, LogType::critical)



enum class LogType: int{
    debug= 0,
    info,
    error,
    critical,
};

/*
int operator<(LogType a, LogType b){
  return static_cast<int>(a)<static_cast<int>(b);
}
int operator>(LogType a, LogType b){
  return static_cast<int>(a)>static_cast<int>(b);
}
*/

class LogMsg {
  public:
    typedef std::shared_ptr<LogMsg> Ptr;
    LogMsg(const std::string & msg_,
           int  line,
           const std::string & file,
           LogType level_ = LogType::info
           ):_msg(msg_),_logLevel(level_),_file(file),_line(line){
       _thread_id = std::this_thread::get_id(); 
    };
    std::thread::id thread_id() const {return _thread_id;};
    static void Log(const std::string & msg_,
                    int                line_,
                    const std::string & file_,
                    LogType level_);
  private:
    std::thread::id  _thread_id;
    std::string      _msg;
    LogType          _logLevel;
    std::string      _file;
    int              _line;

  friend std::ostream & operator<<(std::ostream & os, const LogMsg &);
};


std::ostream & operator<<(std::ostream & os, const LogMsg &);
std::ostream & operator<<(std::ostream &, const LogType&);


class LogDest {
  public:
    enum class Type{
      stdout,
      stderr
    };
    LogDest(Type type);
    void set_log_level(LogType level_){_log_level =  level_;}
    void dispatch(LogMsg::Ptr msgPtr) {_dispatchFunctor(msgPtr);};
  private:
    LogType _log_level;
    std::function<void(LogMsg::Ptr)>  _dispatchFunctor;
};

class Logger {
  public:
    typedef PCQueue<LogMsg::Ptr>        MsgQueue;

    Logger(const Logger & ) = delete;
    Logger(Logger && ) = delete;
    Logger();
    virtual ~Logger();

    void addDest(const LogDest & d);
    void join(){_thread.join();}

    static void send(LogMsg::Ptr logMsg);

  private:
    void run();
    void _sendAync(LogMsg::Ptr);
    std::mutex _map_m;
    MsgQueue           _queue;
    std::atomic_bool  _running;
    std::vector<LogDest>  _dests;
    std::thread  _thread;
};


#endif
