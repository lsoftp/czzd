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

using namespace std;

typedef string STRING;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char BCD;

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
};

class Register
{
public:
	MsgHeader header;
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

	}


};

#endif /* DATASTRUCT_H_ */
