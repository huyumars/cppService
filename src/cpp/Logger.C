#include "Logger.H"
#include <sstream>




void Logger::send(const std::string & msg_,
                    int  line_,
                    const std::string & file_,
                    LogType level_){
  Logger::send( std::make_unique<LogMsg>(msg_,line_,file_,level_));
}



Logger::Logger():_running(true), _thread([this]{ run(); }){ }

void Logger::_addDest(const std::string & key, LogDest::UPtr d){
  _dests[key]=std::move(d);
}

void Logger::run(){
  do{
    Logger::MsgQueue::QueuePtr queuePtr; 
    _queue.getQueue(queuePtr);
    if(queuePtr){
      while(queuePtr->size()){
        for(auto &destPtr : _dests){
          destPtr.second->dispatch(*(queuePtr->front()));
        }
        queuePtr->pop();
      }
    }
  }while(_running);
}

Logger::~Logger(){
  _running = false;
  _queue.stop();
  join();
}

void Logger::send(LogMsg::UPtr msgPtr){
  threads::Singleton<Logger>::instance()._sendAync(std::move(msgPtr));
}


LogDest & Logger::getDest(const std::string &key){
  return *(threads::Singleton<Logger>::instance()._dests[key]);
}

void Logger::addDest(const std::string & key, LogDest::Type type){
  threads::Singleton<Logger>::instance()._addDest(key,std::make_unique<LogDest>(type));
}
void Logger::addDest(const std::string & key, const std::string & fileName){
  threads::Singleton<Logger>::instance()._addDest(key,std::make_unique<LogDest>(fileName));
}


void Logger::_sendAync(LogMsg::UPtr msgPtr){
  _queue.put(std::move(msgPtr));
}

void console(std::ostream& os, const LogMsg& msg,LogType _log_level){
  if(_log_level<=msg.level())
    os<<(msg);
}

LogDest::LogDest(Type type):
   _log_level(LogType::debug){
  switch(type){
    case Type::stderr:
      _dispatchFunctor = [this](const LogMsg& msg){
        console(std::cerr, msg,this->_log_level);
      };
      break;
    case Type::file:
      break;
    case Type::stdout:
    default:
      _dispatchFunctor = [this](LogMsg& msg){
        console(std::cout, msg, this->_log_level);
      };
      break;
  }
}

LogDest::LogDest(const std::string file):LogDest(Type::file)
{
  _logfile = std::ofstream(file,std::ofstream::out);
  _dispatchFunctor = [this](LogMsg& msg){
      console(_logfile,msg, this->_log_level);
  };
}

std::ostream & operator<<(std::ostream &os, const LogType& level){
  switch(level){
    case LogType::info:
      os<<"INFO";
      break;
    case LogType::debug:
      os<<"DEBUG";
      break;
    case LogType::critical:
      os<<"CRITICAL";
      break;
    case LogType::error:
      os<<"ERROR";
      break;
    default:
      break;
  };
  return os;
}

std::ostream & operator<<(std::ostream & os, const LogMsg &msg){
     os<<"["<<msg._thread_id<<"]["<<msg._logLevel<<"]";
     os<<msg._msg;
     os<<"("<<msg._file<<":"<<msg._line<<")"<<std::endl;
     return os;
}



void LogStream::set(int line_, const std::string & fileName_, LogType level_) {
  //clean all
  str("");
  _line = line_;
  _fileName = fileName_;
  _level = level_;
}

LogStream & LogStream::startLogging(int line, const std::string & fileName, LogType _level){
   LogStream & logstream = threads::Singleton<threads::ThreadLocal<LogStream>>::instance();
   logstream.set(line,fileName,_level);
   return logstream;
}

LogStream & operator <<(std::ostream& os, void(*sendFunc)(LogStream &)){
  //it should be LogStream
  LogStream &ls = dynamic_cast<LogStream &>(os); 
  sendFunc(ls);
  return ls;
}
void _logStream_sendfunc(LogStream & logstream){
  Logger::send(logstream.str(),logstream.line(), logstream.fileName(), logstream.type());
}

