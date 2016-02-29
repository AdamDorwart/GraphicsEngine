#pragma once

#include <stdarg.h>

#ifndef LOG_FILE
#define LOG_FILE "log.txt"
#endif

#ifndef NDEBUG
#define LOG_INFO(msg, ...) Logger::info(msg, ## __VA_ARGS__)
#define LOG_COND_INFO(con, msg, ...) Logger::info(con, msg, ## __VA_ARGS__)
#define LOG_ERR(msg, ...) Logger::err(msg, ## __VA_ARGS__)
#else
// Compiler will optimzie these out
#define LOG_INFO(msg, ...) do {} while(false);
#define LOG_COND_INFO(con, msg, ...) do {} while(false);
#define LOG_ERR(msg, ...) do {} while(false);
#endif


class Logger {
  public:
   static char* m_logFile;
   static bool start();
   static bool info(const char* message, ...);
   static bool cond_info(bool cond, const char* message, ...);
   static bool err(const char* message, ...);
};
