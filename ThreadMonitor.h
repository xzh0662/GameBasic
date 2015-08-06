/*
 * ThreadMonitor.h
 *
 *  Created on: Dec 20, 2014
 *      Author: root
 */
/* 1 Thread及其子类不单独创建、运行、停止，统一在ThreadMonitor或者其子类中管理。
 * 2 Thread及其子类可以提供startLoop（线程开始时回调）接口，也可以使用其管理类中的startLoop接口。
 * 3 Thread及其子类应该提供stopLoop接口，统一由其管理类stop时调用。
 */
#ifndef GWEN_THREADMONITOR_H_
#define GWEN_THREADMONITOR_H_

#include "Thread.h"
#include <map>
#include "CommonList.h"
#include "CountDownLatch.h"
#include "Buffer.h"

typedef int (*MSG_HANDEL_FUNC)(void* self, void* msg);

class ThreadMonitor
{
public:
	typedef std::map<pthread_t, Thread*> ThreadMap;

	ThreadMonitor(void);
	virtual ~ThreadMonitor(void);

	virtual int init(void);
	virtual int start(void);
	virtual int stop(void);
	virtual int final(void);

    int bind_thread(Thread *thr);
    int unbind_thread(Thread *thr);

    int find_thread(Thread *&thr, const pthread_t tid = pthread_self());

    static int startLoop(void *self);

protected:
    virtual int startLoop(void) = 0;

	int thread_amount_;
	CommonList *thread_list_;
	CountDownLatch *down_;

private:
    MutexLock mutex_;
    ThreadMap thread_map_;
};

class EventBaseThreadMonitor : public ThreadMonitor
{
public:
	EventBaseThreadMonitor(void);
	virtual ~EventBaseThreadMonitor(void);

	virtual int init(int thread_amount);
	virtual int start(void);
	virtual int stop(void);
	virtual int final(void);

	event_base *eventBase(int index = 0);

protected:
	virtual int startLoop(void);
};

class ProcessThreadMonitor : public ThreadMonitor
{
public:
	ProcessThreadMonitor(void);
	virtual ~ProcessThreadMonitor(void);

	virtual int init(int thread_amount, //
			MSG_HANDEL_FUNC handel_func = NULL, void *handle_prt = NULL);

	virtual int start(void);
	virtual int stop(void);
	virtual int final(void);

	int processPushById(int id, Buffer *buffer);
	int processPush(Buffer* buffer, int index = 0);

	virtual int handleMessage(Buffer* buffer);

	static int startProcessLoop(void *self);

protected:
	static int handleMessage(void* self, void* msg);
	virtual int startLoop(void);


	MSG_HANDEL_FUNC msg_handle_func_;
	void *handle_prt_;
};



#endif /* GWEN_THREADMONITOR_H_ */
