#ifndef LOGGER
#define LOGGER

#include "ThreadSafe.H"
#include <iostream>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <fstream>
#include <functional>
#include <sstream>

#define LOGFUNC(_str_, _level_)  \
    Logger::send(_str_,__LINE__ , __FILE__,_level_)


#define LOGINFO(_str_)  LOGFUNC(_str_, LogType::info)
#define LOGDEBUG(_str_)  LOGFUNC(_str_, LogType::debug)
#define LOGERROR(_str_)  LOGFUNC(_str_, LogType::error)
#define LOGCRITICAL(_str_)  LOGFUNC(_str_, LogType::critical)


#define _STARTLOG(_level_)   \
   LogStream::startLogging (__LINE__,__FILE__,_level_)


#define LogINFO      _STARTLOG(LogType::info)
#define LogDEBUG     _STARTLOG(LogType::debug)
#define LogERROR     _STARTLOG(LogType::error)
#define LogCRITICAL  _STARTLOG(LogType::critical)

#define LogSend     _logStream_sendfunc

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

//support LogInfo <<"hello"<<i<<"world" format

class LogStream : public std::stringstream{
  public:
    static LogStream & startLogging(int line, const std::string & fileName, LogType _level);
    LogStream(){};
    LogStream(const LogStream&)=delete;
    LogStream(LogStream&&)=delete;
    int line() const {return _line;}
    std::string fileName() const {return _fileName;}
    LogType type() const {return _level;}
  private:
    int _line;
    std::string _fileName;
    LogType _level;
    void set(int line, const std::string & fileName, LogType _level);

};

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
LogStream & operator <<(std::ostream& , void(*sendFunc)(LogStream &));

void _logStream_sendfunc(LogStream &);


class LogDest {
  public:
    typedef std::shared_ptr<LogDest>  Ptr;
    enum class Type{
      file,
      stdout,
      stderr
    };
    LogDest(Type type);
    LogDest(const std::string file);
    void set_log_level(LogType level_){_log_level =  level_;}
    void dispatch(LogMsg::Ptr msgPtr) {_dispatchFunctor(msgPtr);};
  private:
    LogType _log_level;
    std::function<void(LogMsg::Ptr)>  _dispatchFunctor;
    std::ofstream  _logfile;
};

class Logger {
  public:
    typedef PCQueue<LogMsg::Ptr>        MsgQueue;

    Logger(const Logger & ) = delete;
    Logger(Logger && ) = delete;
    Logger();
    virtual ~Logger();

    void join(){_thread.join();}

    static void send(const std::string & msg_,
                    int                line_,
                    const std::string & file_,
                    LogType level_);
    static void send(LogMsg::Ptr logMsg);
    static void addDest(LogDest::Type);
    static void addDest(const std::string &);

  private:
    void run();
    void _addDest(LogDest::Ptr);
    void _sendAync(LogMsg::Ptr);
    std::mutex _map_m;
    MsgQueue           _queue;
    std::atomic_bool  _running;
    std::vector<LogDest::Ptr>  _dests;
    std::thread  _thread;
};


#endif
