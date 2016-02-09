#include <stdarg.h>

#ifndef LOG_FILE
#define LOG_FILE "log.txt"
#endif

class Logger {
  public:
   static char* m_logFile;
   static bool start();
   static bool info(const char* message, ...);
   static bool err(const char* message, ...);
};
