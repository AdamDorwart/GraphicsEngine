#include "Logger.h"

#include <time.h>
#include <stdio.h>

char* Logger::m_logFile = LOG_FILE;

bool Logger::start() {
	FILE* file = fopen(m_logFile, "w");
	if (!file) {
		fprintf(stderr, 
				"ERROR: could not open log file %s for writing\n", 
				m_logFile);
		return false;
	}
	time_t now = time(NULL);
	char* date = ctime(&now);
	fprintf (file, "Log File: local time %s\n", date);
	fclose (file);
	return true;
}

bool Logger::info(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen (m_logFile, "a");
	if (!file) {
		fprintf(stderr,
				"ERROR: could not open log file %s file for appending\n",
				m_logFile);
		return false;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	fclose(file);
	return true;
}

bool Logger::err(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen (m_logFile, "a");
	if (!file) {
		fprintf(stderr,
				"ERROR: could not open log file %s file for appending\n",
				m_logFile);
		return false;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	va_start(argptr, message);
	vfprintf(stderr, message, argptr);
	va_end(argptr);
	fclose(file);
	return true;
}