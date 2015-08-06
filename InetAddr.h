/*
 * InetAddr.h
 *
 *  Created on: Dec 11, 2014
 *      Author: root
 */

#ifndef GWEN_INETADDR_H_
#define GWEN_INETADDR_H_

#include <netinet/in.h>
#include <string>

class InetAddr
{
public:
	explicit InetAddr(uint16_t port);

	InetAddr(std::string, uint16_t port);

	InetAddr(const struct sockaddr_in& addr) :
			addr_(addr)
	{
	}

	std::string toIp(void);
	uint16_t toPort(void);

	struct sockaddr_in addr_;
};


#endif /* GWEN_INETADDR_H_ */
