/*
 * TcpServer.h
 *
 *  Created on: Dec 15, 2014
 *      Author: root
 */

#ifndef GWEN_TCPSERVER_H_
#define GWEN_TCPSERVER_H_

#include "InetAddr.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <map>
#include "TcpMonitor.h"
#include "BlockingQueue.h"


template<class Conn>
class TcpServer : public TcpMonitor
{
	typedef std::map<int, Conn*> ConnMap;
public:
	TcpServer(void);

	virtual ~TcpServer(void);

//	virtual int init(int receive_thread_amount,//
//			int send_thread_amount, int event_thread_amount = 1);
//
//	virtual int start(void);
//	virtual int stop(void);
	virtual int final(void);

	virtual int listen(const InetAddr& addr);
	virtual int receive(int fd, Buffer *buffer);
	virtual void handleClosed(int fd);

	virtual int handleListener(int fd);

	static void listenerCallback(struct evconnlistener *listener, //
			int fd, //
			struct sockaddr *sa, //
			int socklen, //
			void *data);

protected:
	Conn* findConnection(int fd);
	virtual int sendMessage(int fd, Buffer *buffer);

private:
	struct evconnlistener *listener_;

	MutexLock mutex_;
	ConnMap conn_map_;//need to lock
};

template<class Conn>
TcpServer<Conn>::TcpServer(void)
	: listener_(NULL)
{
	this->conn_map_.clear();
}

template<class Conn>
TcpServer<Conn>::~TcpServer(void)
{
	this->final();
}

template<class Conn>
void TcpServer<Conn>::listenerCallback(struct evconnlistener *listener, //
		int fd, //
		struct sockaddr *sa, //
		int socklen, //
		void *data)
{
	TcpServer<Conn> *server = (TcpServer<Conn>*)data;
	server->handleListener(fd);
}

//template<class Conn>
//int TcpServer<Conn>::init(int receive_thread_amount, //
//		int send_thread_amount, int event_thread_amount/* = 1*/)
//{
//	return TcpMonitor::init(receive_thread_amount, send_thread_amount, event_thread_amount);
//}
//
//template<class Conn>
//int TcpServer<Conn>::start(void)
//{
//	return TcpMonitor::start();
//}
//
//template<class Conn>
//int TcpServer<Conn>::stop(void)
//{
//	return TcpMonitor::stop();
//}

template<class Conn>
int TcpServer<Conn>::final(void)
{
	if (this->listener_ != NULL)
	{
		evconnlistener_free(this->listener_);
		this->listener_ = NULL;
	}

	return TcpMonitor::final();
}

template<class Conn>
int TcpServer<Conn>::listen(const InetAddr& addr)
{
	unsigned flags = LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE;
	if (this->threadType() == TT_multi_thread)
	{
		flags |= LEV_OPT_THREADSAFE;
	}
	this->listener_ = evconnlistener_new_bind(this->eventBase(), //
			listenerCallback, //
			(void*) this, //
			flags, //
			-1, //
			(struct sockaddr *) &addr.addr_, //
			sizeof(addr.addr_));

	if (this->listener_ == NULL)
	{
		return -1;
	}

	return 0;
}

template<class Conn>
int TcpServer<Conn>::receive(int fd, Buffer *buffer)
{//如果有接收线程则由接收线程回调，否则由event_base 线程调用
	Conn* conn = this->findConnection(fd);
	if (conn == NULL)
	{
		return -1;
	}

	return 0;
}

template<class Conn>
void TcpServer<Conn>::handleClosed(int fd)
{//event_base 线程调用
	MutexLockConditionGuard lock(this->mutex_, this->threadType() == TT_multi_thread);
	this->conn_map_.erase(fd);
}

template<class Conn>
int TcpServer<Conn>::handleListener(int fd)
{//event_base 线程调用
	Conn *conn = new Conn();
	conn->init(fd, this);

	MutexLockConditionGuard lock(this->mutex_, this->threadType() == TT_multi_thread);
	this->conn_map_[fd] = conn;

	return 0;
}

template<class Conn>
Conn* TcpServer<Conn>::findConnection(int fd)
{
	MutexLockConditionGuard lock(this->mutex_, this->threadType() == TT_multi_thread);
	typename ConnMap::iterator iter = this->conn_map_.find(fd);
	if (iter == this->conn_map_.end())
	{
		return NULL;
	}

	return iter->second;
}

template<class Conn>
int TcpServer<Conn>::sendMessage(int fd, Buffer *buffer)
{//如果有发送线程则由发送线程回调，否则由event_base 线程调用
	Conn* conn = this->findConnection(fd);
	if (conn == NULL)
	{
		return -1;
	}

	return conn->send(buffer);
}

#endif /* GWEN_TCPSERVER_H_ */
