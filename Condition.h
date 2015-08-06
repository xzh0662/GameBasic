/*
 * Condition.h
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */

#ifndef GWEN_CONDITION_H_
#define GWEN_CONDITION_H_

#include <pthread.h>
#include "Mutex.h"

class Condition
{
 public:
  explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
  {
    pthread_cond_init(&pcond_, NULL);
  }

  ~Condition()
  {
    pthread_cond_destroy(&pcond_);
  }

  int wait()
  {
	  return pthread_cond_wait(&pcond_, &mutex_.mutex_);
  }


  int notify()
  {
    return pthread_cond_signal(&pcond_);
  }

  int notifyAll()
  {
     return pthread_cond_broadcast(&pcond_);
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t pcond_;
};


#endif /* GWEN_CONDITION_H_ */
