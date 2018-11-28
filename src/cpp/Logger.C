#include "Logger.H"




void LogMsg::Log(const std::string & msg_,
                    int  line_,
                    const std::string & file_,
                    LogType level_){
  Ptr msgPtr = std::make_shared<LogMsg>(msg_,line_,file_,level_);
  Logger::send(msgPtr);
}



Logger::Logger():_running(true), _thread([this]{ run(); }){ }

void Logger::_addDest(LogDest::Ptr d){
  _dests.push_back(d);
}

void Logger::run(){
  do{
    Logger::MsgQueue::QueuePtr queuePtr; 
    _queue.getQueue(queuePtr);
    for(auto destPtr : _dests){
      if(queuePtr){
        while(queuePtr->size()){
          destPtr->dispatch(queuePtr->front());
          queuePtr->pop();
        }
      }
    }
  }while(_running);
}

Logger::~Logger(){
  _running = false;
  _queue.stop();
  join();
}

void Logger::send(LogMsg::Ptr msgPtr){
  Singleton<Logger>::instance()._sendAync(msgPtr);
}


void Logger::addDest(LogDest::Type type){
  Singleton<Logger>::instance()._addDest(std::make_shared<LogDest>(type));
}
void Logger::addDest(const std::string & fileName){
  Singleton<Logger>::instance()._addDest(std::make_shared<LogDest>(fileName));
}


void Logger::_sendAync(LogMsg::Ptr msgPtr){
  _queue.put(msgPtr);
}

void console(std::ostream& os, LogMsg::Ptr msgPtr){
  if(msgPtr) os<<(*msgPtr);
}

LogDest::LogDest(Type type){
  switch(type){
    case Type::stderr:
      _dispatchFunctor = [](LogMsg::Ptr ptr){
        console(std::cerr, ptr);
      };
      break;
    case Type::stdout:
    default:
      _dispatchFunctor = [](LogMsg::Ptr ptr){
        console(std::cout, ptr);
      };
      break;
  }
}

LogDest::LogDest(const std::string file)
{
  _logfile = std::ofstream(file,std::ofstream::out);
  _dispatchFunctor = [this](LogMsg::Ptr ptr){
    console(_logfile,ptr);
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
      os<<"INFO";
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



void LogObject::set(int line_, const std::string & fileName_, LogType level_) {
  _line = line_;
  _fileName = fileName_;
  _level = level_;
}

LogStream & LogObject::fastLogging(int line, const std::string & fileName, LogType _level){
   //LogStream & logstream = Singleton<LogObject>::instance().getStream();
   //logstream.set(line,fileName,_level);
   //return logstream;
}
