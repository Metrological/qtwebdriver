#ifndef WPE_LOGGER_H_
#define WPE_LOGGER_H
#include <iostream>
#include <fstream>

using namespace std;
namespace wpedriver {

enum LogLevel {
    ERR = 1,
    WARN,
    INFO,
    DEBUG,
    TRACE
};

class Logger {

public:
    Logger(): logLevel(ERR), consoleLog(false) {
        file.open("log.txt",  std::fstream::out | std::fstream::trunc);
    }

    Logger(LogLevel level, bool console_log) : logLevel(level), consoleLog(console_log) {
        file.open("log.txt",  std::fstream::out | std::ofstream::trunc);
        minLogLevel = level;
    }

    void Log(LogLevel level, const std::string& message1, const std::string& message2) {
        std::string message;
        message.append(message1);
        message.append(message2);
        Log(level, message);    
    }

    void Log(LogLevel level, const std::string& message) {
        string strApp;
        if(level <= minLogLevel) {

            switch(level){
                case ERR :   strApp = "[ERROR]:" + message;
                             break;
                case WARN :  strApp = "[WARN]:" + message;
                             break;
                case INFO :  strApp = "[INFO]:" + message;
                             break;
                case DEBUG : strApp = "[DEBUG]:" + message;
                             break;
                case TRACE : strApp = "[TRACE]:" + message;
                             break;
                default : break;
            }
        }
        else {
            return;
        }
        file << strApp << endl;
        if (consoleLog) {
            std::cout << strApp  << std::endl;
        }
    }

    virtual ~Logger() {
        file.close();
    }

private:
    bool         consoleLog;
    int          minLogLevel;
    LogLevel     logLevel;
    string       message;
    std::fstream file;

}; //class Logger

} //namespace wpe_driver
#endif // WPE_LOGGER_H_
