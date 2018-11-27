#include "Logger.H"




void LogMsg::Log(const std::string & msg_,
                    int  line_,
                    const std::string & file_,
                    LogType level_){
  Ptr msgPtr = std::make_shared<LogMsg>(msg_,line_,file_,level_);
  Logger::send(msgPtr);
}

Logger::Logger():_running(true), _thread([this]{ run(); }){ }

void Logger::addDest(const LogDest & d){
  _dests.push_back(d);
}

void Logger::run(){
  do{
    Logger::MsgQueue::QueuePtr queuePtr; 
    _queue.getQueue(queuePtr);
    for(LogDest & dest : _dests){
      if(queuePtr){
        while(queuePtr->size()){
          dest.dispatch(queuePtr->front());
          queuePtr->pop();
        }
      }
    }
  }while(_running);
}

Logger::~Logger(){
  _running = false;
  join();
}

void Logger::send(LogMsg::Ptr msgPtr){
  Singleton<Logger>::instance()._sendAync(msgPtr);
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
