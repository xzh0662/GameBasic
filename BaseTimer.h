/*
 * BaseTimer.h
 *
 *  Created on: Dec 16, 2014
 *      Author: root
 */

#ifndef GWEN_BASETIMER_H_
#define GWEN_BASETIMER_H_

#include <event2/event.h>

class BaseTimer
{
public:
	BaseTimer(bool once = false);
	virtual  ~BaseTimer(void);

	void event_base(struct event_base* base);
	int scheduleTimer(int sec, int usec = 0);
	int cancelTimer(void);

	virtual void handleTimeOut(void);

	static void timeOutCallback(int fd, short events, void *data);

private:
	struct event* event_;
	struct event_base* base_;
	int flag_;
};


#endif /* GWEN_BASETIMER_H_ */
