#ifndef __SST_NET_PROTOCOL_H__
#define __SST_NET_PROTOCOL_H__

namespace tdzm
{
	/***********************************************/
	/*网络通信协议数据包头*/
	/*********************************************/
#pragma pack(push, 1)
	struct NetPacketHeader
	{
	public:
		static const unsigned int NetPacketHeaderIdent = 0xAA8899BB;
		static const unsigned int NetLPacketHeaderIdent = 0xAA8899DD;

		unsigned int  dwIdent;//数据包包头标志，固定为NetPacketHeaderIdent
		unsigned short wPacketSize;//数据包长度（不含此包头）
		unsigned short wVerification;//数据效验值，仅客户端向服务器发送的数据包包头中包含对数据段的效验。

	};
	struct ClientNetPacketHeader : public NetPacketHeader
	{
		static const unsigned long ClientNetPacketHeaderIdent = 0xAA8899BC;

		unsigned int dwPacketIndex;//累积数据包索引，客户端每次发送数据包后必须增加此值，服务器检测到此值不比上一次的值大则做丢弃
	};
#pragma pack(pop)

	typedef NetPacketHeader NETPACKETHEADER, *PNETPACKETHEADER;

	/***********************************************/
	/* 计算数据包内容效验值函数*/
	/**********************************************/
	inline unsigned short calcPacketDataVerify(const void* pBuffer, size_t dwSize, unsigned short uKey)
	{
		unsigned short ret = 0x9BCE;
		unsigned char* bv = (unsigned char*)&ret;
		const unsigned char *pb = (const unsigned char*)pBuffer;
		while (dwSize > 0)
		{
			bv[0] ^= *pb;
			bv[1] ^= bv[0];
			dwSize--;
			pb++;
		}
		ret ^= uKey;
		ret ^= ~(dwSize);
		return ret;
	}

	/***********************************************
	 * 客户端数据包内容加密函数
	 **********************************************/
	inline void encryptClientPacket(void *pBuffer, size_t dwSize, unsigned int uKey)
	{
		unsigned int *dw = (unsigned int*)pBuffer;
		unsigned int pv = ((uKey ^ (~(unsigned int)dwSize)) ^ 0xE162C51B);
		while (dwSize > sizeof(*dw))
		{
			pv = *dw ^ (uKey ^ pv);
			*dw = pv;
			dwSize -= sizeof(*dw);
			dw++;
		}
		unsigned char *uc = (unsigned char*)dw;
		while (dwSize > 0)
		{
			*uc ^= (uKey ^ 0xA6);
			uc++;
			dwSize--;
		}
	}

	/***********************************************
	 * 客户端数据包内容解密函数
	 **********************************************/
	inline void decryptClientPacket(void *pBuffer, size_t dwSize, unsigned int uKey)
	{
		unsigned int *dw = (unsigned int*)pBuffer;
		unsigned int pv = ((uKey ^ (~(unsigned int)dwSize)) ^ 0xE162C51B);
		while (dwSize > sizeof(*dw))
		{
			unsigned int temp = *dw;
			*dw ^= (uKey ^ pv);
			pv = temp;
			dwSize -= sizeof(*dw);
			dw++;
		}
		unsigned char *uc = (unsigned char*)dw;
		while (dwSize > 0)
		{
			*uc ^= (uKey ^ 0xA6);
			uc++;
			dwSize--;
		}
	}

	/***********************************************
	 * 客户端数据索包引加密
	 **********************************************/
	inline unsigned int encrpytPacketIndex(unsigned int uPackIndex, unsigned short uKey, unsigned short uLen)
	{
		uPackIndex = uPackIndex ^ (((~uLen) << 16) | uKey);
		unsigned char *sd = (unsigned char*)&uPackIndex;
		sd[3] ^= sd[0];
		sd[2] ^= sd[0];
		sd[1] ^= sd[0];
		return uPackIndex;
	}

	/***********************************************
	 * 客户端数据包索引解密
	 **********************************************/
	inline unsigned int decrpytPacketIndex(unsigned int uPackIndex, unsigned short uKey, unsigned short uLen)
	{
		unsigned char *sd = (unsigned char*)&uPackIndex;
		sd[3] ^= sd[0];
		sd[2] ^= sd[0];
		sd[1] ^= sd[0];
		uPackIndex = uPackIndex ^ (((~uLen) << 16) | uKey);
		return uPackIndex;
	}

	/***********************************************/
	/* 网络通信协议数据包头消息头*/
	/**********************************************/
#pragma pack(push, 1)
	struct NetPacketMessage
	{
		unsigned char btSysId;//系统ID
		unsigned char btCmd;//命令码

		NetPacketMessage(int sysId, int cmd)
		{
			btSysId = (unsigned char)sysId;
			btCmd = (unsigned char)cmd;
		}

		NetPacketMessage(void)
		{
			btSysId = 0;
			btCmd = 0;
		}
	};
#pragma pack(pop)

	typedef NetPacketMessage NETPACKETMSG, *PNETPACKETMSG;

	/************************************************/
	/* 服务器之间通信数据包中传递的用户句柄*/
	/* 如：客户端在逻辑服务器查询角色、创建角色等操作需要由*/
	/* 逻辑服务器转发给DB服务器，未能使逻辑服务器在DB服务器*/
	/* 返回数据后知晓是返回给那个客户端的消息，逻辑服务器会在*/
	/* 通信数据包中包含客户端句柄，DB服务器也会在返回的数据包*/
	/* 中返回此句柄。*/
	/**********************************************/
	struct ServerUserHandle
	{
		unsigned long long llSocket;
		unsigned short wGateIndex;
		unsigned short wUserIndex;
		int nVerify;
	};

	typedef ServerUserHandle SERVERUSERHANDLE, *PSERVERUSERHANDLE;
}

#endif
