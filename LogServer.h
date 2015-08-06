/*
 * LogServer.h
 *
 *  Created on: Jan 14, 2015
 *      Author: root
 */

#ifndef LOGSERVER_H_
#define LOGSERVER_H_
#include "Thread.h"
#include "LogClient.h"
#include "CountDownLatch.h"

class LogServer : Thread
{
public:
	int logPop(char* &msg);
	int logPush(char* msg);

	static LogServer* instance(void);

	int start(const char* path);
	int stop(void);

	void setLevel(int level);
	int level(void);

	bool isOpen(void);

private:
	LogServer(void);
	~LogServer(void);
	static int logFunc(int level, const char *msg, int len);
	static int startLoop(void *self);
	int startLoop(void);

	static LogServer* instance_;

	int level_;
	BlockingQueue log_queue_;
	CountDownLatch *down_;
	bool open_;
};

#define LOG_SERVER LogServer::instance()

#endif /* LOGSERVER_H_ */
