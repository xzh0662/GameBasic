/*
 * CommonList.h
 *
 *  Created on: Dec 18, 2014
 *      Author: root
 */

#ifndef GWEN_COMMONLIST_H_
#define GWEN_COMMONLIST_H_

typedef void* CommonItem;

class CommonList
{
    typedef CommonItem* CommonItemPtr;

public:
    CommonList(int size = 1024);
    virtual ~CommonList(void);

    virtual int add_item(CommonItem item);
    virtual int get_item(CommonItem &item, const int index);

    virtual void reset(void);

    virtual int cur_size(void);
    virtual int max_size(void);

protected:
    virtual int grow(void);

protected:
    int cur_size_;
    int max_size_;
    int def_size_;

    CommonItemPtr items_;
};



#endif /* GWEN_COMMONLIST_H_ */
