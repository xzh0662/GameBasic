/*
 * ThreadMonitor.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: root
 */

#include "ThreadMonitor.h"


ThreadMonitor::ThreadMonitor(void)
	: thread_amount_(0),//
	  thread_list_(NULL),//
	  down_(NULL)
{

}

ThreadMonitor::~ThreadMonitor(void)
{
	printf("ThreadMonitor::~ThreadMonitor\n");
}

int ThreadMonitor::init(void)
{
	this->down_ = new CountDownLatch(this->thread_amount_);

	if (this->down_ == NULL)
	{
		return -1;
	}

	return 0;
}

int ThreadMonitor::start(void)
{
	if (this->down_ == NULL)
	{
		return -1;
	}

	this->down_->wait();

	//printf("ThreadMonitor::started %d\n", (int)time(0));

	return 0;
}

int ThreadMonitor::stop(void)
{
	return 0;
}

int ThreadMonitor::final(void)
{
	if (this->thread_list_ != NULL)
	{
		delete this->thread_list_;
		this->thread_list_ = NULL;
	}

	if (this->down_ != NULL)
	{
		delete this->down_;
		this->down_ = NULL;
	}

	this->thread_amount_ = 0;

	return 0;
}

int ThreadMonitor::bind_thread(Thread *thr)
{
	if (thr == NULL)
	{
		return -1;
	}

	MutexLockGuard lock(mutex_);
	this->thread_map_.insert(std::make_pair(pthread_self(), thr));

	return 0;
}

int ThreadMonitor::unbind_thread(Thread *thr)
{
	if (thr == NULL)
	{
		return -1;
	}

	MutexLockGuard lock(mutex_);
	this->thread_map_.erase(pthread_self());

	return 0;
}

int ThreadMonitor::startLoop(void *self)
{
	ThreadMonitor *monitor = (ThreadMonitor*)self;

	monitor->down_->countDown();

	return monitor->startLoop();
}

int ThreadMonitor::find_thread(Thread *&thr, const pthread_t tid/* = pthread_self()*/)
{
	MutexLockGuard lock(mutex_);

	ThreadMap::const_iterator iter = this->thread_map_.find(tid);
	if (iter == this->thread_map_.end())
	{
		thr = NULL;
		return -1;
	}

	thr = iter->second;

	return 0;
}


EventBaseThreadMonitor::EventBaseThreadMonitor(void)
{

}

EventBaseThreadMonitor::~EventBaseThreadMonitor(void)
{
	printf("EventBaseThreadMonitor::~EventBaseThreadMonitor\n");
	this->final();
}

int EventBaseThreadMonitor::init(int thread_amount)
{
	if (this->thread_list_ != NULL)
	{// init tiwce
		return -1;
	}

	if (thread_amount <= 0)
	{
		return -1;
	}

	this->thread_amount_ = thread_amount;

	this->thread_list_ = new CommonList(this->thread_amount_);
	EventBaseThread *event_base_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		event_base_thread = new EventBaseThread(this);
		event_base_thread->threadIndex(i);
		this->thread_list_->add_item(event_base_thread);
	}

	return ThreadMonitor::init();
}

int EventBaseThreadMonitor::start(void)
{
	if (this->thread_list_ == NULL)
	{
		return -1;
	}

	EventBaseThread *event_base_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		this->thread_list_->get_item((CommonItem&)event_base_thread, i);
		event_base_thread->start();
	}

	return ThreadMonitor::start();
}

int EventBaseThreadMonitor::stop(void)
{
	if (this->thread_list_ == NULL)
	{
		return -1;
	}

    EventBaseThread *event_base_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		this->thread_list_->get_item((CommonItem&)event_base_thread, i);
		event_base_thread->stopEventBaseLoop();
		event_base_thread->wait();
	}

	return 0;
}

int EventBaseThreadMonitor::final(void)
{
	if (this->thread_list_ != NULL)
	{
	    EventBaseThread *event_base_thread;
	    for (int i = 0; i < this->thread_amount_; i++)
	    {
	        this->thread_list_->get_item((CommonItem&) event_base_thread, i);
	        delete event_base_thread;
	    }
	    this->thread_list_ = NULL;
	}


    return ThreadMonitor::final();
}

event_base *EventBaseThreadMonitor::eventBase(int index/* = 0*/)
{
	if (index >= this->thread_amount_)
	{
		return NULL;
	}

	EventBaseThread *event_base_thread;
	this->thread_list_->get_item((CommonItem&) event_base_thread, index);
	if (event_base_thread == NULL)
	{
		return NULL;
	}

	return event_base_thread->eventBase();
}



int EventBaseThreadMonitor::startLoop(void)
{
	EventBaseThread *event_base_thread;
	if (this->find_thread((Thread*&) event_base_thread) != 0)
	{
		return -1;
	}

	event_base_thread->startEventBaseLoop();

	return 0;
}

ProcessThreadMonitor::ProcessThreadMonitor(void)
	: msg_handle_func_(NULL), //
	  handle_prt_(NULL)
{

}

ProcessThreadMonitor::~ProcessThreadMonitor(void)
{
	printf("ProcessThreadMonitor::~ProcessThreadMonitor\n");
	this->final();
}

int ProcessThreadMonitor::init(int thread_amount, //
		MSG_HANDEL_FUNC handel_func, void *handle_prt)
{
	if (this->thread_list_ != NULL)
	{// init tiwce
		return -1;
	}

	if (thread_amount <= 0)
	{
		return -1;
	}

	if (handel_func == NULL)
	{
		this->msg_handle_func_ = ProcessThreadMonitor::handleMessage;
		this->handle_prt_ = this;
	}
	else
	{
		this->msg_handle_func_ = handel_func;
		this->handle_prt_ = handle_prt;
	}

	this->thread_amount_ = thread_amount;

	this->thread_list_ = new CommonList(this->thread_amount_);
	ProcessThread *process_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		process_thread = new ProcessThread(this);
		process_thread->threadIndex(i);
		this->thread_list_->add_item(process_thread);
	}

	return ThreadMonitor::init();
}

int ProcessThreadMonitor::start(void)
{
	if (this->thread_list_ == NULL || this->msg_handle_func_ == NULL)
	{
		return -1;
	}

	ProcessThread *process_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		this->thread_list_->get_item((CommonItem&)process_thread, i);
		process_thread->start();
	}

	return ThreadMonitor::start();
}

int ProcessThreadMonitor::stop(void)
{
	if (this->thread_list_ == NULL || this->msg_handle_func_ == NULL)
	{
		return -1;
	}

    ProcessThread *process_thread;
	for (int i = 0; i < this->thread_amount_; i++)
	{
		this->thread_list_->get_item((CommonItem&)process_thread, i);
		process_thread->stopProcessLoop();
		process_thread->wait();
	}

	return 0;
}

int ProcessThreadMonitor::final(void)
{
	if (this->thread_list_ != NULL)
	{
		ProcessThread *process_thread;
		for (int i = 0; i < this->thread_amount_; i++)
		{
			this->thread_list_->get_item((CommonItem&) process_thread, i);
			delete process_thread;
		}

		this->thread_list_ = NULL;
	}

    return ThreadMonitor::final();
}

int ProcessThreadMonitor::processPushById(int id, Buffer *buffer)
{
	if (id < 0)
	{
		id = 0;
	}

	return this->processPush(buffer, id % this->thread_amount_);
}

int ProcessThreadMonitor::processPush(Buffer* buffer, int index/* = 0*/)
{
	if (this->thread_list_ == NULL || this->msg_handle_func_ == NULL)
	{
		return -1;
	}

	ProcessThread *process_thread;
	this->thread_list_->get_item((CommonItem&) process_thread, index);
	if (process_thread == NULL)
	{
		return -1;
	}

	return process_thread->processPush(buffer);
}

int ProcessThreadMonitor::handleMessage(Buffer* buffer)
{
	return 0;
}

int ProcessThreadMonitor::handleMessage(void* self, void* msg)
{
	ProcessThreadMonitor *monitor = (ProcessThreadMonitor*)self;
	return monitor->handleMessage((Buffer*)msg);
}

int ProcessThreadMonitor::startLoop(void)
{
	ProcessThread *process_thread;
	if (this->find_thread((Thread*&) process_thread) != 0)
	{
		return -1;
	}

	Buffer *buffer = NULL;

	while (true)
	{
		if (process_thread->processPop(buffer) != -1)
		{
			if (buffer == NULL)
			{
				break;
			}

			this->msg_handle_func_(this->handle_prt_, (void*)buffer);

			delete buffer;
		}
		else
		{
			printf("process_thread->processPop(msg) == -1\n");
			break;
		}
	}

	printf("start_process_loop over tid = %d\n", (int)pthread_self());

	return 0;
}
