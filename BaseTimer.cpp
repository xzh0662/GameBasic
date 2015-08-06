/*
 * BaseTimer.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: root
 */


#include "BaseTimer.h"

BaseTimer::BaseTimer(bool once)
		: event_(NULL),
		  base_(NULL)
{
	if (once)
	{
		this->flag_ = 0;
	}
	else
	{
		this->flag_ = EV_PERSIST;
	}
}

BaseTimer::~BaseTimer(void)
{
	if (event_ != NULL)
	{
		this->cancelTimer();
	}
}

void BaseTimer::event_base(struct event_base* base)
{
	this->base_ = base;
}

int BaseTimer::scheduleTimer(int sec, int usec/*=0*/)
{
	this->event_ = event_new(this->base_, -1, this->flag_, timeOutCallback, (void*)this);
	if (this->event_ == NULL)
	{
		return -1;
	}

	timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;

	if (event_add(this->event_, &tv) != 0)
	{
		event_del(this->event_);
		return -1;
	}

	return 0;
}

int BaseTimer::cancelTimer(void)
{
	if (this->event_ != NULL)
	{
		return event_del(this->event_);
		this->event_ = NULL;
	}

	return 0;
}

void BaseTimer::handleTimeOut(void)
{
	printf("BaseTimer::handleTimeOut\n");
}


void BaseTimer::timeOutCallback(int fd, short events, void *data)
{
	BaseTimer *timer = (BaseTimer*)data;

	timer->handleTimeOut();

	if (timer->flag_ == 0)
	{
		timer->event_ = NULL;
	}
}
