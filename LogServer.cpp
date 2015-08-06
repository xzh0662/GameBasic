/*
 * LogServer.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: root
 */

#include "LogServer.h"

int LogServer::logPop(char* &msg)
{
	return this->log_queue_.peek((CommonItem&) msg);;
}

int LogServer::logPush(char* msg)
{
	return this->log_queue_.append((CommonItem)msg);
}

LogServer* LogServer::instance_ = 0;

LogServer* LogServer::instance(void)
{
    if (instance_ == 0)
    {
        instance_ = new LogServer();
    }
    return instance_;
}


int LogServer::start(const char* path)
{
	this->down_ = new CountDownLatch(1);

	if (this->down_ == 0)
	{
		return -1;
	}

	if (Thread::start()== -1)
	{
		delete this->down_;

		return -1;
	}

	this->down_->wait();

	if (LogClient::logFunc() == NULL)
	{
		LogClient::setLogFunc(LogServer::logFunc);
	}

	//open file

	this->open_ = true;

	return 0;
}

int LogServer::stop(void)
{
	this->log_queue_.appendNull();

	//close file
	this->open_ = false;

	return 0;
}

void LogServer::setLevel(int level)
{
	this->level_ = level;
}

int LogServer::level(void)
{
	return this->level_;
}

bool LogServer::isOpen(void)
{
	return this->open_;
}

LogServer::LogServer(void)
	: level_(0),//
	  log_queue_(false),//
	  down_(0),//
	  open_(false)
{
	this->loop_ptr_ = this;
	this->start_loop_ = LogServer::startLoop;
}

LogServer::~LogServer(void)
{
	if (this->down_ != 0)
	{
		delete this->down_;
		this->down_ = 0;
	}
}

int LogServer::logFunc(int level, const char *msg, int len)
{
	if (len <=0)
	{
		return -1;
	}

	printf("%s\n", msg);

	if (LOG_SERVER->isOpen())
	{
		if (level >= LOG_SERVER->level())
		{
			char *log_msg = new char[len + 1];
			log_msg[len] = '\0';

			LOG_SERVER->logPush(log_msg);
		}
	}

	return 0;
}

int LogServer::startLoop(void *self)
{
	return ((LogServer*)self)->startLoop();
}

int LogServer::startLoop(void)
{
	this->down_->countDown();
	char *msg = 0;
	while (true)
	{
		if (this->logPop(msg) != -1)
		{
			if (msg == 0)
			{
				break;
			}

			//1 前端时间格式化
			//2 根据当前时间新起文件。
			//3 记录当前文件。


			delete msg;
		}
		else
		{
			LOG_DEBUG << "this->logPop(msg) == -1";

			break;
		}
	}

	return 0;
}

