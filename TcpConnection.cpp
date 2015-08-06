/*
 * TcpConnection.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: root
 */

#include "TcpConnection.h"
#include "TcpMonitor.h"
#include <stdio.h>//printf
#include <event2/event.h>

TcpConnection::RecyleTimer::RecyleTimer(void)
	: BaseTimer(true),
	  ptr_(NULL)
{

}

TcpConnection::RecyleTimer::~RecyleTimer(void)
{

}

void TcpConnection::RecyleTimer::handleTimeOut(void)
{
	printf("RecyleTimer::handleTimeOut\n");

	delete this->ptr_;
}

TcpConnection::TcpConnection(void)
	  : bevent_(NULL),//
		owner_(NULL),//
		fd_(-1)
{
}

TcpConnection::~TcpConnection(void)
{
	printf("~TcpConnection\n");
}

int TcpConnection::init(int fd, //
		TcpMonitor *owner, bool isClient/* = false*/)
{
	if (this->bevent_ != NULL || owner == NULL)
	{
		return -1;
	}

	int options = BEV_OPT_CLOSE_ON_FREE;
	if (owner->threadType() == TT_multi_thread)
	{
		options |= BEV_OPT_THREADSAFE;
	}

	int newFd = isClient ? -1 : fd;
	this->bevent_ = bufferevent_socket_new(owner->eventBase(), newFd, options);
	if (!this->bevent_)
	{
		printf("Error constructing bufferevent!\n");

		return -1;
	}
	else
	{
		bufferevent_setcb(this->bevent_, readCallback, NULL, eventCallback, (void*) this);
		bufferevent_enable(this->bevent_, EV_READ);

		this->owner_ = owner;
		this->fd_ = fd;
	}

	return 0;
}

struct bufferevent* TcpConnection::bevent(void)
{
	return this->bevent_;
}

void TcpConnection::handleConnected(void)
{
	printf("connection connected\n");
	this->owner_->handleConnected();
}

void TcpConnection::handleRead(void)
{
	if (this->bevent_ == NULL)
	{
		return;
	}

	Buffer *buffer = new Buffer();
	int ret = bufferevent_read_buffer(this->bevent_, buffer->evb_);
	if (ret != 0)
	{
		return;
	}

	this->owner_->handleRead(this->fd_, buffer);
}

void TcpConnection::handleClosed(void)
{
	printf("connection closed\n");
}

void TcpConnection::handleClear(void)
{
	printf("connection clear\n");

	this->owner_->handleClosed(this->fd_);

	//这将自动close套接字和free读写缓冲区
	bufferevent_free(this->bevent_);

	this->recyle_timer_.event_base(this->owner_->eventBase());
	this->recyle_timer_.ptr_ = this;
	this->recyle_timer_.scheduleTimer(3,0);
}

int TcpConnection::send(const Buffer *buf)
{
	if (this->bevent_ == NULL)
	{
		return -1;
	}

	return bufferevent_write_buffer(this->bevent_, buf->evb_);
}

int TcpConnection::fd(void)
{
	return this->fd_;
}

void TcpConnection::readCallback(struct bufferevent *bev, void *data)
{
	TcpConnection *conn = (TcpConnection*)data;
	conn->handleRead();
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *data)
{
	//暂时没有处理
}

void TcpConnection::eventCallback(struct bufferevent *bev, short events, void *data)
{
	TcpConnection *conn = (TcpConnection*)data;

	if (events & BEV_EVENT_EOF)
	{
		conn->handleClosed();
	}
	else if(events & BEV_EVENT_CONNECTED)
	{
		conn->handleConnected();
		return;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("some other error\n");
	}

	conn->handleClear();
}


