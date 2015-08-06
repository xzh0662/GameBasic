/*
 * CommonList.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */

#include "CommonList.h"
#include "stdlib.h"
#include "string.h"

CommonList::CommonList(int size) :
    cur_size_(0), max_size_(0), items_(0)
{
    this->def_size_ = size > 0 ? size : 1024;
}

CommonList::~CommonList(void)
{
    if (this->items_ != 0)
    {
        free(this->items_);
        this->items_ = 0;
    }
}

int CommonList::add_item(CommonItem item)
{
//    if (item == 0)
//    {
//        return -1;
//    }
    if (this->cur_size_ < this->max_size_ || this->grow() == 0)
    {
        this->items_[this->cur_size_] = item;
        return this->cur_size_++;
    }
    return -1;
}

int CommonList::get_item(CommonItem &item, const int index)
{
    if (index < this->cur_size_ && index >= 0)
    {
        item = this->items_[index];
        return 0;
    }
    return -1;
}

void CommonList::reset(void)
{
    this->cur_size_ = 0;
    memset(this->items_, 0, sizeof (CommonItem) * this->max_size_);
}

int CommonList::cur_size(void)
{
    return this->cur_size_;
}

int CommonList::max_size(void)
{
    return this->max_size_;
}

int CommonList::grow(void)
{
    if (this->items_ == 0 || this->max_size_ == 0)
    {
        this->items_ = (CommonItemPtr) malloc(sizeof (CommonItem) * this->def_size_);
        memset(this->items_, 0, sizeof (CommonItem) * this->def_size_);
        this->max_size_ = this->def_size_;
    }
    else
    {
        this->items_ = (CommonItemPtr) realloc(this->items_, sizeof (CommonItem) * (this->max_size_ + this->def_size_));
        memset((char*) this->items_ + sizeof (CommonItem) * this->max_size_, 0, sizeof (CommonItem) * this->def_size_);
        this->max_size_ += this->def_size_;
    }
    if (this->items_ != 0)
    {
        return 0;
    }
    return -1;
}
