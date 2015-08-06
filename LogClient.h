/*
 * LogClient.h
 *
 *  Created on: Jan 14, 2015
 *      Author: root
 */

#ifndef LOGCLIENT_H_
#define LOGCLIENT_H_

#include <stdio.h>
#include <string>
#include <sstream>

typedef int (*LogFunc)(int, const char *, int);

template<typename OUT, typename IN>
OUT convert(const IN in)
{
    std::stringstream ss;
    OUT out;
    ss << in;
    ss >> out;
    return out;
}

class LogClient
{
public:
	enum LogLevel
	{
		DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS,
	};

	LogClient(const char* file, int line, const char* func, int logLevel);
	~LogClient(void);

	static void setLogFunc(LogFunc log_func);
	static LogFunc logFunc(void);

	LogClient& operator <<(const char * cstr);
	LogClient& operator <<(const double val);
	LogClient& operator <<(const char c);
	LogClient& operator <<(const bool val);
	LogClient& operator <<(const int &val);
	LogClient& operator <<(const long int &val);
	LogClient& operator <<(const long long int &val);
	LogClient& operator <<(const std::string &ss);
	LogClient& operator <<(const short &val);


private:
	int level_;

	static LogFunc log_func_;
	std::string log_str_;
};

#define LOG_DEBUG LogClient( __FILE__, __LINE__, __func__, LogClient::DEBUG)
#define LOG_INFO LogClient( __FILE__, __LINE__, __func__, LogClient::INFO)
#define LOG_WARN LogClient( __FILE__, __LINE__, __func__, LogClient::WARN)
#define LOG_ERROR LogClient( __FILE__, __LINE__, __func__, LogClient::ERROR)
#define LOG_FATAL LogClient( __FILE__, __LINE__, __func__, LogClient::FATAL)

#endif /* LOGCLIENT_H_ */
