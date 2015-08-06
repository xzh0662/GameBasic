/*
 * InetAddr.cpp
 *
 *  Created on: Dec 11, 2014
 *      Author: root
 */

#include "InetAddr.h"

#include <string.h>
#include <endian.h>
#include <arpa/inet.h>
#include <stdio.h>

InetAddr::InetAddr(uint16_t port)
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	in_addr_t ip = INADDR_ANY;
	addr_.sin_addr.s_addr = htobe32(ip);
	addr_.sin_port = htobe16(port);
}

InetAddr::InetAddr(std::string ip, uint16_t port)
{
	addr_.sin_family = AF_INET;
	addr_.sin_port = htobe16(port);
	if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)
	{
		printf("ip format error !\n");
	}
}

std::string InetAddr::toIp(void)
{
	char buf[32];
	::inet_ntop(AF_INET, &addr_.sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
	return buf;
}

uint16_t InetAddr::toPort(void)
{
	return be16toh(addr_.sin_port);
}



