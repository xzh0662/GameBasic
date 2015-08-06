/*
 * GlobalInfo.h
 *
 *  Created on: Dec 19, 2014
 *      Author: root
 */

#ifndef GWEN_GLOBALINFO_H_
#define GWEN_GLOBALINFO_H_

#include <event2/event.h>
#include <event2/thread.h>
#include <stdio.h>
#include <string.h>
#include "Mutex.h"

class GlobalInfo
{
public:
	static void print(void)
	{
		printf("eventSupportedMethods:\n");
		const char** all_methods = eventSupportedMethods();
		while (all_methods && *all_methods)
		{
			printf("%s\t", *all_methods++);
		}

		printf("\n");

		event_base *base = event_base_new();
		if (base)
		{
			printf("eventUsingMethod:\t %s\n", eventUsingMethod(base));
			printf("eventBaseFeatures:\t %d\n", eventBaseFeatures(base));

			event_base_free(base);
		}
	}

	static int useThreads(void)
	{//使多线程支持该程序使用，在程序开始时调用。
		return evthread_use_pthreads();
	}

	static const char ** eventSupportedMethods(void)
	{//当前系统所支持的多路IO复用函数有哪些
		return event_get_supported_methods();
	}

	static const char * eventUsingMethod(const struct event_base *base)
	{//该函数返回值是对应event_base* 当前所采用的多路IO复用函数是哪个
		return event_base_get_method(base);
	}

	static int eventBaseFeatures(const struct event_base *base)
	{//event_base当前所采用的特征是什么
		return event_base_get_features(base);
	}

	static MutexLock tm_time_mutex;

	static int gLocaltime(const time_t time, struct tm *p)
	{
		if (p == 0)
		{
			return -1;
		}
		memset(p, 0, sizeof(struct tm));

		MutexLockGuard lock(tm_time_mutex);
		::localtime_r(&time, p);//not thread safe

		return 0;
	}

	static struct tm changeToTm(const time_t time)
	{
	    struct tm p;
	    gLocaltime(time, &p);
	    return p;
	}
};



#endif /* GWEN_GLOBALINFO_H_ */
