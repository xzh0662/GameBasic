/*
 * BlockingQueue.h
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */

#ifndef GWEN_BLOCKINGQUEUE_H_
#define GWEN_BLOCKINGQUEUE_H_

#include "CommonList.h"
#include "Condition.h"

class BlockingQueue
{
public:
	BlockingQueue(bool read_lock = true, int size = 1024);

	~BlockingQueue(void);

    int append(CommonItem item);
    int peek(CommonItem &item);

    int appendNull(void);//use threadmonitor

private:
    int swap(CommonItem &item);
    int peek_detail(CommonItem &item);
    int append_detail(CommonItem item);


    CommonList *read_list_;
    CommonList *write_list_;
    MutexLock read_mutex_;
    MutexLock write_mutex_;
    Condition listEmpty_;

    int peek_index_;
    bool read_lock_;
};



#endif /* GWEN_BLOCKINGQUEUE_H_ */
