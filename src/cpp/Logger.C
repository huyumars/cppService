#include "Logger.H"




void LogMsg::Log(const std::string & msg_,
                    int  line_,
                    const std::string & file_,
                    LogType level_){
  Ptr msgPtr = std::make_shared<LogMsg>(msg_,line_,file_,level_);
  Singleton<Logger>::instance().send(msgPtr);
}

void Logger::send(LogMsg::Ptr msgPtr){
  std::cout<<(*msgPtr);
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
