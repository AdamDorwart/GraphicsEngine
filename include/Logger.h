#pragma once

#include <stdarg.h>

#ifndef LOG_FILE
#define LOG_FILE "log.txt"
#endif

#define LogError(msg, ...) Logger::err(msg, ## __VA_ARGS__)

#ifndef NDEBUG
#define LogInfo(msg, ...) Logger::info(msg, ## __VA_ARGS__)
#else
// Compiler will optimzie these out
#define LogInfo(msg, ...) 
#endif

class Logger {
  public:
   static char* m_logFile;
   static bool start();
   static bool info(const char* message, ...);
   static bool err(const char* message, ...);
};
