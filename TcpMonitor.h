/*
 * TcpMonitor.h
 *
 *  Created on: Dec 17, 2014
 *      Author: root
 */

#ifndef GWEN_TCPMONITOR_H_
#define GWEN_TCPMONITOR_H_

#include <string.h>//use strlen by TcpClient and TcpServer
#include "ThreadMonitor.h"
#include "Buffer.h"

enum ThreadType
{
	TT_single_thread = 1,
	TT_multi_thread = 2,
};

struct HalfBlock
{
	Buffer buf;
	bool headIsOver;
	unsigned int needSize;
};

class TcpMonitor
{
	typedef std::map<int, HalfBlock*> HalfMap;
public:
	TcpMonitor(void);
	virtual ~TcpMonitor(void);

	virtual int init(int receive_thread_amount = 0 ,//
			int send_thread_amount = 0, int event_thread_amount = 0);

	virtual int start(void);
	virtual int stop(void);
	virtual int final(void);

	void handleRead(int fd, Buffer *buffer);
	virtual int send(int fd, Buffer *buffer);
	virtual int receive(int fd, Buffer *buffer) =0;
	virtual void handleClosed(int fd) = 0;
	virtual void handleConnected(void){};

	virtual int receiveHandleMessage(Buffer *buffer);
	virtual int sendHandleMessage(Buffer *buffer);

	event_base* eventBase(int index = 0);
	void eventBase(event_base* base);

	int threadType(void);

protected:
	virtual int sendMessage(int fd, Buffer *buffer) = 0;

private:

	static int receiveHandleMessage(void *self, void *msg);
	static int sendHandleMessage(void *self, void *msg);

	void errorDeleteThreadMonitor(ThreadMonitor* &monitor, const char *error = NULL);

	bool makeCompletionMessage(int fd, Buffer *inBuffer, Buffer *&outBuffer);
	bool makeHeadCompletionMessage(const tdzm::NetPacketHeader& head, //
			int fd, Buffer *inBuffer, Buffer *&outBuffer);

	event_base* main_base_;

	ProcessThreadMonitor *receive_thread_monitor_;
	ProcessThreadMonitor *send_thread_monitor_;
	EventBaseThreadMonitor *event_thread_monitor_;

	int thread_type_;

	MutexLock m_halfMutex;
	HalfMap m_halfMap;
};



#endif /* GWEN_TCPMONITOR_H_ */
