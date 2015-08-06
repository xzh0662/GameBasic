/*
 * TcpMonitor.cpp
 *
 *  Created on: Dec 24, 2014
 *      Author: root
 */

#include "TcpMonitor.h"

TcpMonitor::TcpMonitor(void)
	: main_base_(NULL),//
	  receive_thread_monitor_(NULL),//
	  send_thread_monitor_(NULL),//
	  event_thread_monitor_(NULL),
	  thread_type_(0)
{
}

TcpMonitor::~TcpMonitor(void)
{
}

int TcpMonitor::init(int receive_thread_amount/* = 0*/, //
		int send_thread_amount/* = 0*/, int event_thread_amount/* = 0*/)
{
	if (this->main_base_ == NULL)
	{
		if (event_thread_amount <= 0)
		{
			return -1;
		}

		this->event_thread_monitor_ = new EventBaseThreadMonitor();
		if (this->event_thread_monitor_ == NULL)
		{
			return -1;
		}

		if (this->event_thread_monitor_->init(event_thread_amount) != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->event_thread_monitor_,//
								"event_thread_monitor_ init failed");

			return -1;
		}
	}

	if (receive_thread_amount > 0)
	{
		this->receive_thread_monitor_ = new ProcessThreadMonitor();
		if (this->receive_thread_monitor_ == NULL)
		{
			return -1;
		}

		int ret = this->receive_thread_monitor_->init(receive_thread_amount,//
				TcpMonitor::receiveHandleMessage, (void*)this);

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->receive_thread_monitor_,//
					"receive_thread_monitor_ init failed");

			return ret;
		}
	}

	if (send_thread_amount > 0)
	{
		this->send_thread_monitor_ = new ProcessThreadMonitor();
		if (this->send_thread_monitor_ == NULL)
		{
			return -1;
		}

		int ret = this->send_thread_monitor_->init(send_thread_amount,//
					TcpMonitor::sendHandleMessage, (void*)this);

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->send_thread_monitor_,//
					"send_thread_monitor_ init failed");

			return ret;
		}
	}

	if (this->receive_thread_monitor_ == NULL//
			&& this->send_thread_monitor_ == NULL//
			&& this->event_thread_monitor_ == NULL)
	{
		this->thread_type_ = TT_single_thread;
	}
	else
	{
		this->thread_type_ = TT_multi_thread;
	}

	return 0;
}

int TcpMonitor::start(void)
{
	if (this->main_base_ == NULL)
	{
		if (this->event_thread_monitor_ == NULL)
		{
			return -1;
		}

		if (this->event_thread_monitor_->start() != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->event_thread_monitor_,//
								"event_thread_monitor_ start failed");
			return -1;
		}
	}

	if (this->receive_thread_monitor_ != NULL)
	{
		int ret = this->receive_thread_monitor_->start();

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->receive_thread_monitor_,//
					"receive_thread_monitor_ start failed");

			return ret;
		}
	}

	if (this->send_thread_monitor_ != NULL)
	{
		int ret = this->send_thread_monitor_->start();

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->send_thread_monitor_,//
					"send_thread_monitor_ start failed");

			return ret;
		}
	}

	return 0;
}

int TcpMonitor::stop(void)
{
	if (this->main_base_ == NULL)
	{
		if (this->event_thread_monitor_ == NULL)
		{
			return -1;
		}

		if (this->event_thread_monitor_->stop() != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->event_thread_monitor_,//
								"event_thread_monitor_ stop failed");
			return -1;
		}
	}

	if (this->receive_thread_monitor_ != NULL)
	{
		int ret = this->receive_thread_monitor_->stop();

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->receive_thread_monitor_,//
					"receive_thread_monitor_ stop failed");

			return ret;
		}
	}

	if (this->send_thread_monitor_ != NULL)
	{
		int ret = this->send_thread_monitor_->stop();

		if (ret != 0)
		{
			this->errorDeleteThreadMonitor((ThreadMonitor*&)this->send_thread_monitor_,//
					"send_thread_monitor_stop failed");

			return ret;
		}
	}

	return 0;
}

int TcpMonitor::final(void)
{
	this->main_base_ = NULL;

	if (this->event_thread_monitor_ != NULL)
	{
		this->event_thread_monitor_->final();

		delete this->event_thread_monitor_;
		this->event_thread_monitor_ = NULL;
	}

	if (this->receive_thread_monitor_ != NULL)
	{
		this->receive_thread_monitor_->final();

		delete this->receive_thread_monitor_;
		this->receive_thread_monitor_ = NULL;
	}

	if (this->send_thread_monitor_ != NULL)
	{
		this->send_thread_monitor_->final();

		delete this->send_thread_monitor_;
		this->send_thread_monitor_ = NULL;
	}

	return 0;
}

void TcpMonitor::handleRead(int fd, Buffer *buffer)
{//如果有接收线程，则所有buffer由接收线程处理并销毁，否则由当前线程处理并销毁。
	buffer->addBeginInt(fd);
	if (this->receive_thread_monitor_ != NULL)
	{
		//printf("TcpMonitor::handleRead with thread\n");
		this->receive_thread_monitor_->processPushById(fd, buffer);
	}
	else
	{
		this->receiveHandleMessage(buffer);
		delete buffer;
	}
}

int TcpMonitor::send(int fd, Buffer *buffer)
{
	buffer->addBeginInt(fd);
	if (this->send_thread_monitor_ != NULL)
	{
		//printf("TcpMonitor::send with thread\n");
		this->send_thread_monitor_->processPushById(fd, buffer);
	}
	else
	{
		this->sendHandleMessage(buffer);
		delete buffer;
	}

	return 0;
}

int TcpMonitor::receiveHandleMessage(Buffer *buffer)
{//这里做粘包,解密处理
	int fd = 0;
	*buffer >> fd;
	//printf("TcpMonitor::receiveHandleMessage  fd = %d\n", fd);

	Buffer *completeBuffer = NULL;
	while(makeCompletionMessage(fd, buffer, completeBuffer))
	{
		if (completeBuffer)
		{
			this->receive(fd, completeBuffer);
			delete completeBuffer;
			completeBuffer = NULL;
		}
	}

	return 0;
}

int TcpMonitor::sendHandleMessage(Buffer *buffer)
{//这里做加密处理
	int fd = 0;
	*buffer >> fd;

	this->sendMessage(fd, buffer);

	return 0;
}

event_base* TcpMonitor::eventBase(int index/* = 0*/)
{
	if (this->main_base_)
	{
		return this->main_base_;
	}

	if (this->event_thread_monitor_)
	{
		return this->event_thread_monitor_->eventBase(index);
	}

	return NULL;
}

void TcpMonitor::eventBase(event_base* base)
{
	if (this->event_thread_monitor_)
	{
		return;
	}

	this->main_base_ = base;
}

int TcpMonitor::threadType(void)
{
	return this->thread_type_;
}

int TcpMonitor::receiveHandleMessage(void *self, void *msg)
{
	TcpMonitor* monitor = (TcpMonitor*)self;
	return monitor->receiveHandleMessage((Buffer*)msg);
}

int TcpMonitor::sendHandleMessage(void *self, void *msg)
{
	TcpMonitor* monitor = (TcpMonitor*)self;
	return monitor->sendHandleMessage((Buffer*)msg);
}

void TcpMonitor::errorDeleteThreadMonitor(ThreadMonitor* &monitor, const char *error)
{
	if (error == NULL)
	{
		printf("errorDeleteProcessThreadMonitor error\n");
	}
	else
	{
		printf("errorDeleteProcessThreadMonitor error : %s \n", error);
	}

	if (monitor != NULL)
	{
		delete monitor;
		monitor = NULL;
	}
}

bool TcpMonitor::makeCompletionMessage(int fd, Buffer *inBuffer, Buffer *&outBuffer)
{
	MutexLockConditionGuard lock(this->m_halfMutex, this->threadType() == TT_multi_thread);
	if (inBuffer->Length() == 0)
	{
		return false;
	}

	HalfMap::iterator iter = this->m_halfMap.find(fd);
	if (iter == this->m_halfMap.end())
	{
		if (inBuffer->Length() >= sizeof(tdzm::NetPacketHeader))
		{
			tdzm::NetPacketHeader head;
			*inBuffer >> head;
			return this->makeHeadCompletionMessage(head, fd, inBuffer, outBuffer);
		}
		else
		{
			HalfBlock *halfBlock = new HalfBlock();
			halfBlock->needSize = sizeof(tdzm::NetPacketHeader) - inBuffer->Length();
			halfBlock->headIsOver = false;
			evbuffer_add_buffer(halfBlock->buf.evb_, inBuffer->evb_);
			this->m_halfMap.insert(std::make_pair(fd, halfBlock));
			//printf("head is not complete fd=%d, need head size=%d\n", fd, halfBlock->needSize);
			return false;
		}
	}
	else
	{
		HalfBlock *halfBlock = iter->second;
		if (halfBlock->headIsOver)
		{
			if (inBuffer->Length() >=halfBlock->needSize)
			{
				evbuffer_remove_buffer(inBuffer->evb_, halfBlock->buf.evb_, halfBlock->needSize);
				outBuffer = new Buffer();

				tdzm::NetPacketHeader head;
				halfBlock->buf >> head;

				evbuffer_add_buffer(outBuffer->evb_, halfBlock->buf.evb_);

				this->m_halfMap.erase(iter);

				//printf("all is complete fd=%dbuff size=%d\n", fd, (int)outBuffer->Length());

				delete halfBlock;

				return true;
			}
			else
			{
				halfBlock->needSize -= inBuffer->Length();
				evbuffer_add_buffer(halfBlock->buf.evb_, inBuffer->evb_);

				//printf("times head is complete fd=%d, need buff size=%d\n", fd, halfBlock->needSize);
				return false;
			}
		}
		else
		{
			if (inBuffer->Length() >=halfBlock->needSize)
			{
				evbuffer_remove_buffer(inBuffer->evb_, halfBlock->buf.evb_,
						halfBlock->needSize);

				tdzm::NetPacketHeader head;
				halfBlock->buf >> head;

				this->m_halfMap.erase(iter);

				//printf("head is complete fd=%d buff size=%d\n", fd, (int)halfBlock->buf.Length());

				delete halfBlock;

				return this->makeHeadCompletionMessage(head, fd, inBuffer, outBuffer);
			}
			else
			{
				halfBlock->needSize -= inBuffer->Length();
				evbuffer_add_buffer(halfBlock->buf.evb_, inBuffer->evb_);

				//printf("times head is not complete fd=%d, need head size=%d\n", fd, halfBlock->needSize);
				return false;
			}
		}
	}
}

bool TcpMonitor::makeHeadCompletionMessage(const tdzm::NetPacketHeader& head, //
		int fd, Buffer *inBuffer, Buffer *&outBuffer)
{
	if (inBuffer->Length() >= head.wPacketSize)
	{
		outBuffer = new Buffer();
//		tdzm::encryptClientPacket()
		evbuffer_remove_buffer(inBuffer->evb_, outBuffer->evb_,
				head.wPacketSize);

		return true;
	}
	else
	{
		HalfBlock *halfBlock = new HalfBlock();

		halfBlock->buf << head;
		halfBlock->needSize = head.wPacketSize - inBuffer->Length();
		halfBlock->headIsOver = true;
		evbuffer_add_buffer(halfBlock->buf.evb_, inBuffer->evb_);
		this->m_halfMap.insert(std::make_pair(fd, halfBlock));
		//printf("head is complete fd=%d, need buff size=%d\n", fd,
		//		halfBlock->needSize);

		return false;
	}
}



