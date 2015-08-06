/*
 * CountDownLatch.h
 *
 *  Created on: Dec 20, 2014
 *      Author: root
 */

#ifndef GWEN_COUNTDOWNLATCH_H_
#define GWEN_COUNTDOWNLATCH_H_

#include "Condition.h"

class CountDownLatch
{
public:

	explicit CountDownLatch(int count);

	void wait();

	void countDown();

	//void setCount(int count);
	int getCount() const;

private:
	mutable MutexLock mutex_; //mutable <---- const
	Condition condition_;
	int count_;
};

#endif /* GWEN_COUNTDOWNLATCH_H_ */
