/*
 * Thread.h
 *
 *  Created on: Dec 20, 2014
 *      Author: root
 */
/* 1 Thread及其子类不单独创建、运行、停止，统一在ThreadMonitor或者其子类中管理。 except signalthread
 * 2 Thread及其子类可以提供startLoop（线程开始时回调）接口，也可以使用其管理类中的startLoop接口。
 * 3 Thread及其子类应该提供stopLoop接口，统一由其管理类stop时调用。
 */

#ifndef GWEN_THREAD_H_
#define GWEN_THREAD_H_

#include <event2/event.h>
#include <event2/event_compat.h>
#include "BlockingQueue.h"
#include <set>

class ThreadMonitor;
class Buffer;

typedef int (*START_LOOP_FUNC)(void *arg);

class Thread
{
public:
	Thread(ThreadMonitor *monitor = NULL);
     ~Thread(void);

    static void* threadCallback(void *arg);

    int threadIndex(int index = -1);

    int start(void);
    int wait(void);
    int signal(int sig);

    int runStartLoop(void);

protected:
    ThreadMonitor *monitor_;
    START_LOOP_FUNC start_loop_;
    void *loop_ptr_;
    pthread_t tid_;

    int thread_index_;
};

class EventBaseThread : public Thread
{
public:
	EventBaseThread(ThreadMonitor *monitor);
	~EventBaseThread(void);

	event_base *eventBase(void);

	int startEventBaseLoop(void);
	int stopEventBaseLoop(void);

private:
	static void eventCallback(int fd, short which, void *arg);

protected:
	struct event_base *base_;
	struct event *event_;	//监听管理的事件机
	int receive_fd_;		//管理的接收端
	int send_fd_;			//管道的发送端
};

class SignalEventBaseThread: public Thread
{
	typedef std::set<event *> SignalEventSet;
public:
	SignalEventBaseThread(void);
	virtual ~SignalEventBaseThread(void);

	event_base *eventBase(void);

	virtual int signalCallBack(int sig);

	void registeSignal(int sig);

	int startSignalLoop(void);

private:
	static void eventCallback(int fd, short which, void *arg);
	static int startSignalLoop(void *arg);

private:
	struct event_base *base_;
	SignalEventSet sig_set_;
};

class ProcessThread : public Thread
{
public:
	ProcessThread(ThreadMonitor *monitor);
	~ProcessThread(void);

	int processPop(Buffer* &buffer);
	int processPush(Buffer* buffer);

	int stopProcessLoop(void);

protected:
	BlockingQueue process_queue_;
};


#endif /* GWEN_THREAD_H_ */
