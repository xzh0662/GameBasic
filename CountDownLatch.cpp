/*
 * CountDownLatch.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: root
 */
#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) :
		mutex_(), condition_(mutex_), count_(count)
{
}

void CountDownLatch::wait()
{
	MutexLockGuard lock(mutex_);
	while (count_ > 0)
	{
		condition_.wait();
	}
}

void CountDownLatch::countDown()
{
	MutexLockGuard lock(mutex_);
	--count_;
	if (count_ == 0)
	{
		condition_.notifyAll();
	}
}

//void CountDownLatch::setCount(int count)
//{
//	MutexLockGuard lock(mutex_);
//	this->count_ = count;
//}

int CountDownLatch::getCount() const
{
	MutexLockGuard lock(mutex_);
	return count_;
}


