/*
 * Thread.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: root
 */


#include "Thread.h"
#include "ThreadMonitor.h"

Thread::Thread(ThreadMonitor *monitor)
	: monitor_(monitor),//
	  start_loop_(NULL),//
	  loop_ptr_(NULL),//
	  tid_(0), //
	  thread_index_(-1)
{

}

Thread::~Thread(void)
{

}

void* Thread::threadCallback(void *arg)
{
	Thread* thread = (Thread*)arg;

	if (thread->monitor_ != NULL)
	{
		thread->monitor_->bind_thread(thread);
	}

	thread->runStartLoop();

	if (thread->monitor_ != NULL)
	{
		thread->monitor_->unbind_thread(thread);
	}

	return NULL;
}

int Thread::threadIndex(int index/* = -1*/)
{
	if (index >= 0)
	{
		this->thread_index_ = index;
	}

	return this->thread_index_;
}

int Thread::start(void)
{
	if (pthread_create(&this->tid_, NULL, threadCallback, (void*)this) != 0)
	{
		return -1;
	}

	return 0;
}

int Thread::wait(void)
{
	return pthread_join(this->tid_, NULL);
}

int Thread::runStartLoop(void)
{
	if (this->start_loop_ == NULL)
	{
		return -1;
	}

	if (this->loop_ptr_ == NULL)
	{
		return -1;
	}

	return this->start_loop_(this->loop_ptr_);
}

EventBaseThread::EventBaseThread(ThreadMonitor *monitor)
	: Thread(monitor),//
	  event_(NULL),//
	  receive_fd_(0),//
	  send_fd_(0)

{
	this->base_ = event_base_new();
	this->loop_ptr_ = monitor;
	this->start_loop_ = ThreadMonitor::startLoop;
}

EventBaseThread::~EventBaseThread(void)
{
	if (this->base_ != NULL)
	{
		event_base_free(this->base_);
		this->base_ = NULL;
	}

	if (this->event_ != NULL)
	{
		event_del(this->event_);
		event_free(this->event_);
	}

	if (this->receive_fd_ != 0)
	{
		close(this->receive_fd_);
	}

	if (this->send_fd_ != 0)
	{
		close(this->send_fd_);
	}
}

event_base *EventBaseThread::eventBase(void)
{
	return this->base_;
}

int EventBaseThread::startEventBaseLoop(void)
{
	if (this->eventBase() == NULL)
	{
		return -1;
	}

	int fds[2];
	if (pipe(fds) != 0)
	{
		printf("EventBaseThread::startEventBaseLoop error\n");
		return -1;
	}

	this->receive_fd_ = fds[0];
	this->send_fd_ = fds[1];

	this->event_ = event_new(this->base_, this->receive_fd_, //
			EV_READ| EV_PERSIST, eventCallback, (void*)this);

	if (this->event_ == NULL)
	{
		printf("EventBaseThread::startEventBaseLoop error\n");
		return -1;
	}

	if (event_add(this->event_, NULL) != 0)
	{
		printf("EventBaseThread::startEventBaseLoop error\n");
		event_del(this->event_);
		return -1;
	}

	event_base_dispatch(this->base_);

	printf("EventBaseThread::startEventBaseLoop over\n");

	return 0;
}

int EventBaseThread::stopEventBaseLoop(void)
{
	if (this->base_ == NULL)
	{
		return -1;
	}

	char content = 0;//暂时通知只做中断处理

	write(this->send_fd_, &content, sizeof(char));

	return 0;
}

void EventBaseThread::eventCallback(int fd, short which, void *arg)
{//暂时通知只做中断处理
	EventBaseThread* thread = (EventBaseThread*)arg;

	event_base_loopbreak(thread->base_);
}

SignalEventBaseThread::SignalEventBaseThread(void)
	: base_(NULL)
{
	this->loop_ptr_ = this;
	this->start_loop_ = SignalEventBaseThread::startSignalLoop;
}

SignalEventBaseThread::~SignalEventBaseThread(void)
{
	for (SignalEventSet::iterator iter = this->sig_set_.begin();//
			iter != this->sig_set_.end(); ++iter)
	{
		event_del(*iter);
	}

	if (this->base_ != NULL)
	{
		event_base_free(this->base_);
		this->base_ = NULL;
	}
}

event_base *SignalEventBaseThread::eventBase(void)
{
	if (this->base_ == NULL)
	{
		this->base_ = event_base_new();
	}

	return this->base_;
}

int SignalEventBaseThread::signalCallBack(int sig)
{
	return event_base_loopbreak(this->base_);
}

void SignalEventBaseThread::registeSignal(int sig)
{
	event* ev = evsignal_new(this->eventBase(), sig, eventCallback, (void* )this);
	if (!ev || event_add(ev, NULL) < 0)
	{
		event_del(ev);
		printf("error add sig = %d \n", sig);
	}
	this->sig_set_.insert(ev);
}

int SignalEventBaseThread::startSignalLoop(void)
{
	return event_base_dispatch(this->eventBase());
}

void SignalEventBaseThread::eventCallback(int fd, short which, void *arg)
{
	SignalEventBaseThread* thread = (SignalEventBaseThread*)arg;

	thread->signalCallBack(fd);

}

int SignalEventBaseThread::startSignalLoop(void *arg)
{
	SignalEventBaseThread* thread = (SignalEventBaseThread*)arg;
	return thread->startSignalLoop();
}

ProcessThread::ProcessThread(ThreadMonitor *monitor)
	: Thread(monitor),//
	  process_queue_(false)
{
	this->loop_ptr_ = monitor;

	this->start_loop_ = ThreadMonitor::startLoop;
}

ProcessThread::~ProcessThread(void)
{

}

int ProcessThread::processPop(Buffer* &buffer)
{
	return this->process_queue_.peek((CommonItem&) buffer);
}

int ProcessThread::processPush(Buffer* buffer)
{
	return this->process_queue_.append((CommonItem)buffer);
}

int ProcessThread::stopProcessLoop(void)
{
	return this->process_queue_.appendNull();
}


