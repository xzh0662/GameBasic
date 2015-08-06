/*
 * Mutex.h
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */

#ifndef GWEN_MUTEX_H_
#define GWEN_MUTEX_H_

#include <pthread.h>

class MutexLock
{
	friend class Condition;
public:
	MutexLock(void)
	{
		pthread_mutex_init(&mutex_, NULL);
	}

	~MutexLock(void)
	{
		pthread_mutex_destroy(&mutex_);
	}

	int lock()
	{
		return pthread_mutex_lock(&mutex_);
	}

	int unlock()
	{
		return pthread_mutex_unlock(&mutex_);
	}

private:
	  pthread_mutex_t mutex_;
};

class MutexLockGuard
{
public:
	explicit MutexLockGuard(MutexLock& mutex) :
			mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard()
	{
		mutex_.unlock();
	}

private:
	MutexLock& mutex_;
};

class MutexLockConditionGuard
{
public:
	MutexLockConditionGuard(MutexLock& mutex, bool is_lock) :
			mutex_(mutex),//
			is_lock_(is_lock)
	{
		if (is_lock_)
		{
			mutex_.lock();
		}
	}

	~MutexLockConditionGuard()
	{
		if (is_lock_)
		{
			mutex_.unlock();
		}
	}

private:
	MutexLock& mutex_;
	bool is_lock_;
};

#define MutexLockConditionGuard(x) error "Missing guard object name"
#define MutexLockGuard(x) error "Missing guard object name"



#endif /* GWEN_MUTEX_H_ */
