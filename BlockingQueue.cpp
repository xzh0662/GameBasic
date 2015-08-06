/*
 * BlockingQueue.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */


#include "BlockingQueue.h"

BlockingQueue::BlockingQueue(bool read_lock, int size) :
		 read_mutex_(), //
		 write_mutex_(), //
		 listEmpty_(write_mutex_), //
		 peek_index_(0), //
		 read_lock_(read_lock)
{
	this->read_list_ = new CommonList(size);
	this->write_list_ = new CommonList(size);
}

BlockingQueue::~BlockingQueue(void)
{
    delete this->read_list_;
    delete this->write_list_;
}

int BlockingQueue::append(CommonItem item)
{
	if (item == NULL)
	{
		return -1;
	}

	MutexLockGuard lock(this->write_mutex_);

    return this->append_detail(item);
}

int BlockingQueue::peek(CommonItem &item)
{
	MutexLockConditionGuard lock(this->read_mutex_, this->read_lock_);
	return this->peek_detail(item);
}

int BlockingQueue::swap(CommonItem &item)
{
	MutexLockGuard lock(this->write_mutex_);

	while (this->write_list_->cur_size() == 0)
	{
		listEmpty_.wait();
	}

	if (this->write_list_->cur_size() == 0)
	{
		return -1;
	}

	CommonList *temp_list;
	temp_list = this->read_list_;
	temp_list->reset();
	this->read_list_ = this->write_list_;
	this->write_list_ = temp_list;
	this->peek_index_ = 0;

	return this->read_list_->get_item(item, this->peek_index_++);
}

int BlockingQueue::peek_detail(CommonItem &item)
{
	if (this->peek_index_ < this->read_list_->cur_size())
	{
		return this->read_list_->get_item(item, this->peek_index_++);
	}
	else
	{
		return this->swap(item);
	}
}

int BlockingQueue::append_detail(CommonItem item)
{
	int ret = this->write_list_->add_item(item);

	if (ret == 0)
	{
		listEmpty_.notify();
	}

	return ret;
}

int BlockingQueue::appendNull(void)
{
	return this->append_detail(NULL);
}
