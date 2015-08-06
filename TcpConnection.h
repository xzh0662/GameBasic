/*
 * TcpConnection.h
 *
 *  Created on: Dec 15, 2014
 *      Author: root
 */

#ifndef GWEN_TCPCONNECTION_H_
#define GWEN_TCPCONNECTION_H_

#include <event2/util.h>
#include <event2/bufferevent.h>
#include "BaseTimer.h"
#include "Buffer.h"

class TcpMonitor;

class TcpConnection
{
public:
	class RecyleTimer : public BaseTimer
	{
	public:
		RecyleTimer(void);
		virtual  ~RecyleTimer(void);

		virtual void handleTimeOut(void);

		TcpConnection *ptr_;
	};

public:
	TcpConnection(void);

	virtual ~TcpConnection(void);

	virtual int init(int fd, //
			TcpMonitor *owner, bool isClient = false);

	struct bufferevent* bevent(void);

	virtual void handleConnected(void);
	virtual void handleRead(void);
	virtual void handleClosed(void);
	virtual void handleClear(void);

	virtual int send(const Buffer *buf);

	int fd(void);

	static void readCallback(struct bufferevent *bev, void *data);
	static void writeCallback(struct bufferevent *bev, void *data);
	static void eventCallback(struct bufferevent *bev, short events, void *data);

protected:

	RecyleTimer recyle_timer_;

	struct bufferevent *bevent_;

	TcpMonitor *owner_;
	int fd_;
};




#endif /* GWEN_TCPCONNECTION_H_ */
