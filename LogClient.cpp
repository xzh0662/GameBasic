/*
 * LogClient.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: root
 */


#include "LogClient.h"

static const char* LogLevelName[LogClient::NUM_LOG_LEVELS] =
{
	"DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};


LogClient::LogClient(const char* file, int line, const char* func, int logLevel)
	: level_(logLevel)
{
	this->log_str_ += "(";
	this->log_str_ += LogLevelName[logLevel];
	this->log_str_ += ")file:";
	this->log_str_ += file;
	this->log_str_ += " line:";
	this->log_str_ += convert<std::string>(line);
	this->log_str_ += " func:";
	this->log_str_ += func;
	this->log_str_ += " -->";
}

LogClient::~LogClient(void)
{
	if (LogClient::log_func_ == NULL)
	{
		printf("%s\n", this->log_str_.c_str());
	}
	else
	{
		LogClient::log_func_(level_, this->log_str_.c_str(), this->log_str_.length());
	}
}

LogFunc LogClient::log_func_ = 0;
void LogClient::setLogFunc(LogFunc log_func)
{
	log_func_ = log_func;
}

LogFunc LogClient::logFunc(void)
{
	return log_func_;
}

LogClient& LogClient::operator << (const char * cstr)
{
	this->log_str_ += cstr;
	return *this;
}

LogClient& LogClient::operator << (const double val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}

LogClient& LogClient::operator <<(const char c)
{
	this->log_str_ += convert<std::string>(c);
	return *this;
}

LogClient& LogClient::operator <<(const bool val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}

LogClient& LogClient::operator <<(const int &val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}

LogClient& LogClient::operator <<(const long int &val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}

LogClient& LogClient::operator <<(const long long int &val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}
LogClient& LogClient::operator <<(const std::string &ss)
{
	this->log_str_ += ss;
	return *this;
}

LogClient& LogClient::operator <<(const short &val)
{
	this->log_str_ += convert<std::string>(val);
	return *this;
}


