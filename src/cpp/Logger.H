#ifndef LOGGER
#define LOGGER

#include "ThreadSafe.H"
#include <iostream>
#include <thread>

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

class Logger {
  public:
    static void send(LogMsg::Ptr logMsg);
  private:
};


#endif
