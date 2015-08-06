/*
 * TcpClient.h
 *
 *  Created on: Dec 15, 2014
 *      Author: root
 */

#ifndef GWEN_TCPCLIENT_H_
#define GWEN_TCPCLIENT_H_

#include "InetAddr.h"
#include <event2/bufferevent.h>
#include "TcpMonitor.h"

#include "stdio.h"

template<class Conn>
class TcpClient : public TcpMonitor
{
public:
	TcpClient(void);

	virtual ~TcpClient();

//	virtual int init(int receive_thread_amount,//
//			int send_thread_amount, int event_thread_amount = 1);
//
//	virtual int start(void);
//	virtual int stop(void);
	virtual int final(void);

	virtual int connect(const InetAddr& serverAddr);
	virtual int receive(int fd, Buffer *buffer);
	virtual void handleClosed(int fd);
	virtual void handleConnected(void);

protected:
	virtual int sendMessage(int fd, Buffer *buffer);

private:
	Conn *conn_;
	CountDownLatch *down_;
	//bool connect_;
};

template<class Conn>
TcpClient<Conn>::TcpClient(void)
	: conn_(0),//
	  down_(0)
{

}

template<class Conn>
TcpClient<Conn>::~TcpClient()
{
	this->final();
}

//template<class Conn>
//int TcpClient<Conn>::init(int receive_thread_amount, //
//		int send_thread_amount, int event_thread_amount/* = 1*/)
//{
//	return TcpMonitor::init(receive_thread_amount, send_thread_amount, event_thread_amount);;
//}
//
//template<class Conn>
//int TcpClient<Conn>::start(void)
//{
//	return TcpMonitor::start();
//}
//
//template<class Conn>
//int TcpClient<Conn>::stop(void)
//{
//	return TcpMonitor::stop();
//}

template<class Conn>
int TcpClient<Conn>::final(void)
{
	if (this->conn_ != 0)
	{
		delete this->conn_;
		this->conn_ = 0;
	}
	return TcpMonitor::final();
}

template<class Conn>
int TcpClient<Conn>::connect(const InetAddr& serverAddr)
{
	if (this->conn_ == NULL)
	{
		this->conn_ = new Conn();
		if (this->conn_->init(-1, this))
		{
			delete this->conn_;
			return -1;
		}
	}

	if (this->threadType() == TT_multi_thread)
	{
		this->down_ = new CountDownLatch(1);
	}

	int ret = bufferevent_socket_connect(this->conn_->bevent(),//
			(struct sockaddr *)&serverAddr.addr_,//
			sizeof(serverAddr.addr_));

	if (this->down_)
	{
		this->down_->wait();
	}

	return ret;
}

template<class Conn>
int TcpClient<Conn>::receive(int fd, Buffer *buffer)
{
	return 0;
}

template<class Conn>
void TcpClient<Conn>::handleClosed(int fd)
{
	this->conn_ = 0;

	if (this->down_)
	{
		this->down_->countDown();
		delete this->down_;
	}

	printf("TcpClient<Conn>::handleClosed fd = %d\n", fd);
}

template<class Conn>
void TcpClient<Conn>::handleConnected(void)
{
	if (this->down_)
	{
		this->down_->countDown();
		delete this->down_;
		this->down_ = NULL;
	}

	printf("TcpClient<Conn>::handleConnected\n");
}

template<class Conn>
int TcpClient<Conn>::sendMessage(int fd, Buffer *buffer)
{
	if (this->conn_ == 0)
	{
		return -1;
	}
	return this->conn_->send(buffer);
}

#endif /* GWEN_TCPCLIENT_H_ */
