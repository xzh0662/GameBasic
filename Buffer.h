/*
 * Buffer.h
 *
 *  Created on: Dec 25, 2014
 *      Author: root
 */

#ifndef GWEN_BUFFER_H_
#define GWEN_BUFFER_H_

#include <event2/buffer.h>
#include <string>
#include "SSTNetProtocol.h"

#define BUFFER_NORMAL_SIZE 1024

enum Endian
{
	little_endian = 1,
	big_endian = 2
};

class Buffer
{//暂时不支持线程安全

	friend class TcpConnection;
	friend class TcpMonitor;
public:
	Buffer(int endian = little_endian);
	~Buffer(void);

	size_t Length(void) const;

	Buffer & operator <<(const char c);
	Buffer & operator <<(const unsigned char c);
	Buffer & operator <<(const bool val);
	Buffer & operator <<(const int &val);
	Buffer & operator <<(const long int &val);
	Buffer & operator <<(const long long int &val);
	Buffer & operator <<(const unsigned int &val);
	Buffer & operator <<(const char *cstr);
	Buffer & operator <<(const std::string &ss);
	Buffer & operator <<(const short &val);
	Buffer & operator <<(const unsigned short &val);
	Buffer & operator <<(const double &val);
	Buffer & operator <<(const tdzm::NetPacketHeader &val);
	Buffer & operator <<(const tdzm::ClientNetPacketHeader &val);
	Buffer & operator <<(const tdzm::NetPacketMessage &val);

	Buffer & operator >>(char &c);
	Buffer & operator >>(unsigned char &c);
	Buffer & operator >>(bool &val);
	Buffer & operator >>(int &val);
	Buffer & operator >>(long int &val);
	Buffer & operator >>(long long int &val);
	Buffer & operator >>(unsigned int &val);
	Buffer & operator >>(char *&cstr);
	Buffer & operator >>(std::string &ss);
	Buffer & operator >>(short &val);
	Buffer & operator >>(unsigned short &val);
	Buffer & operator >>(double &val);
	Buffer & operator >>(tdzm::NetPacketHeader &val);
	Buffer & operator >>(tdzm::ClientNetPacketHeader &val);
	Buffer & operator >>(tdzm::NetPacketMessage &val);

	Buffer & operator =(const Buffer &buffer);

	void addBeginInt(const int &val);

	int expand(const int size);
	int move(Buffer *srcBuffer);

	int addData(const void *data, size_t len);
	int removeData(void *data, size_t len);

private:
	struct evbuffer *evb_;
	int endian_;
};

#define BUFFER_NEW_ACTION(BUFFER, SIZE, ACTION)	\
	do {	\
		BUFFER = new Buffer();	\
		if (BUFFER->expand(SIZE)){ACTION;}	\
	}while(0)	\

#define BUFFER_NEW(BUFFER, SIZE) \
	BUFFER_NEW_ACTION(BUFFER, SIZE, return)	\

#define BUFFER_NEW_RETURN(BUFFER, SIZE, RETURN) \
	BUFFER_NEW_ACTION(BUFFER, SIZE, return RETURN)	\


#endif /* GWEN_BUFFER_H_ */
