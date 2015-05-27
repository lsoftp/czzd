/*
 * datastruct.h
 *
 *  Created on: 2015Äê5ÔÂ25ÈÕ
 *      Author: Administrator
 */

#ifndef DATASTRUCT_H_
#define DATASTRUCT_H_

#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <list>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/select.h>
#include "pthread.h"

using namespace std;

typedef string STRING;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char BCD;

struct Msg
{
	unsigned char stream[1024];
	int len;
	int resendTimes;
	WORD msgSerialNumber;
	int sendChars;
	struct timeval sendTime;
	bool complete;
};

struct RecvStream
{
	unsigned char stream[1560];
	int size;
};
class RecvBuf
{
public:
	unsigned char stream[1560];
	int size;
	RecvBuf()
	{
		size = 0;
	}
	int getDataFromBuf(unsigned char *buf, int *len)
	{
		int j,begin = -1,end=-1;
		for (j=0;j<size;j++)
		{
			if (stream[j] == 0x7e)
			{
				if(begin == -1)
				{
					begin=j;
				}
				else
				{
					end=j;
				}
			}
		}
		if(end == -1) //not found end of the packet
		{
			memmove(stream, stream+begin, size - begin);
			size = size -begin;
			return -2;
		}
		if((size -end -1) >0)
		{
			memcpy(buf, stream+begin, end-begin +1);
			memmove(stream, stream+end+1,size - end-1);
			size = size -end -1;
			*len = end -begin +1;
			return *len;
		}
		else if ((size - end -1) == 0)
		{
			memcpy(buf, stream+begin, size-begin);
			size=0;
			*len = size -begin;
			return *len;
		}
		else
		{
			*len = 0;
			return -1;
		}
	}
};

struct MsgHeader
{
	WORD msgId;
	WORD property;
	BCD  phoneNumber[6];
	WORD msgSerialNumber;
	WORD packetCount;
	WORD packetNo;
	int toStream(unsigned char * original)
	{
		int j = 0;
		WORD tmp;
		tmp = htons(msgId);
		memcpy(original, &tmp, sizeof(WORD));
		j += sizeof(WORD);
		tmp = htons(property);
		memcpy(original+j, &tmp,2);
		j += 2;
		memcpy(original+j, phoneNumber,6);
		j += 6;
		tmp =htons(msgSerialNumber);
		memcpy(original+j, &tmp, 2);
		j += 2;
		if(property& 0x2000 )
		{
			tmp = htons(packetCount);
			memcpy(original +j, &tmp,2);
			j += 2;
			tmp = htons(packetNo);
			memcpy(original + j, &tmp,2);
			j += 2;
		}
		return j;
	}
	int fromStream(unsigned char * original)
	{
		int j =0;
		msgId=ntohs(*(WORD*)(original+j));
		j += 2;
		property = ntohs(*(WORD *)(original+j));
		j += 2;
		memcpy(&phoneNumber,original+j,6);
		j += 6;
		msgSerialNumber = ntohs(*(WORD*)(original+j));
		j += 2;
		if(property & 0x2000)
		{
			packetCount = ntohs(*(WORD *)(original+j));
			j += 2;
			packetNo = ntohs(*(WORD*)(original+j));
			j += 2;
		}
		return j;
	}
};

class Register
{
public:
	MsgHeader header; //0x0100
	WORD provinceId;
	WORD cityId;
	BYTE manufacturerId[5];
	BYTE type[8];
	BYTE terminalId[7];
	BYTE plateColor;
	STRING plateNumber;

	Register()
	{

	}
	int toStream(unsigned char * original)
	{
		int j;
		j = header.toStream(original);
		WORD tmp;
		tmp = htons(provinceId);
		memcpy(original +j , &tmp,2);
		j += 2;
		tmp = htons(cityId);
		memcpy(original + j, &tmp,2);
		j += 2;
		memcpy(original + j, manufacturerId,5);
		j += 5;
		memcpy(original + j, type, 8);
		j += 8;
		memcpy(original + j, terminalId, 7);
		j += 7;
		memcpy(original + j, &plateColor, 1);
		j += 1;
		memcpy(original + j, plateNumber.c_str(), plateNumber.length()+1);
		j =  j + plateNumber.length()+1;
		//return j;

	}


};

class Authentication
{
public:
	MsgHeader header; //0x0102
	STRING code;
	int toStream(unsigned char * original)
	{
		int j;
		j = header.toStream(original);
		memcpy(original+j, code.c_str(),code.length()+1);
		j = j + code.length() + 1;
		return j;
	}
};

class RegisterAck
{
public:
	MsgHeader header; //0x8100
	BYTE result;
	STRING authenticationCode;

	int fromStream(unsigned char* original, int len)
	{
		int j = 0;
		j = header.fromStream(original);
		result = *(original+j);
		j += 1;
		if(0 == result)
		{
			authenticationCode = (char*)original+j;
			j = j + authenticationCode.length() + 1;
		}
		return j;

	}

};

#endif /* DATASTRUCT_H_ */
