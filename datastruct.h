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
#include <exception>
#include <stdexcept>
#include <vector>
#include "pthread.h"
#include "datatype.h"
#include "config.h"

#define BUFFER_SIZE 1560

using namespace std;



struct Msg
{
	unsigned char stream[BUFFER_SIZE];
	int len;
	int resendTimes;
	WORD msgSerialNumber;
	int sendChars;
	struct timeval sendTime;
	bool complete;
	bool isAck;
	Msg()
	{
		isAck = false;
		resendTimes = 0;
		complete = false;
		sendChars = 0;
	}
};

struct RecvStream
{
	unsigned char stream[BUFFER_SIZE];
	int size;
};
class RecvBuf
{
public:
	unsigned char stream[BUFFER_SIZE];
	int size;
	RecvBuf()
	{
		size = 0;
	}
    int getDataFromBuf(unsigned char *buf, int *len)
    {
        int j,begin = -1,end=-1;
        if(size == 0)
        {
            *len = 0;
            return 0;
        }

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
                    break;
                }
            }
        }
        if(end == begin +1)
        {
            memmove(stream, stream+end, size - end);
            size = size -end;
            return 0;
        }
        if((begin != -1)&&(end == -1)) //not found end of the packet
        {
            memmove(stream, stream+begin, size - begin);
            size = size -begin;
            return 0;
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

            *len = size -begin;
            size=0;
            return *len;
        }
        else
        {
            *len = 0;
            return 0;
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

class TerminalAck
{
public:
	MsgHeader header; //0x0001
	WORD serialNumber;
	WORD msgId;
	BYTE result; //0 success ,1 failure, 2 invalid msg 3 not support

	TerminalAck()
	{
		header.msgId = 0x0001;
	}

	int toStream(unsigned char * original)
	{
		int j;
		j = header.toStream(original);
		WORD tmp;
		tmp = htons(serialNumber);
		memcpy(original + j, &tmp, 2);
		j += 2;
		tmp = htons(msgId);
		memcpy(original + j, &tmp, 2);
		j += 2;
		memcpy(original + j, &result, 1);
		j += 1;
		return j;
	}

};

class PlatformAck
{
public:
	MsgHeader header; //0x8001
	WORD serialNumber;
	WORD msgId;
	BYTE result; //0 success ,1 failure, 2 invalid msg 3 not support


	int fromStream(unsigned char * original)
	{
		int j = 0;
		j = header.fromStream(original);
		serialNumber=ntohs(*(WORD*)(original+j));
		j += 2;
		msgId = ntohs(*(WORD *)(original+j));
		j += 2;
		result = *(BYTE *)(original+j);
		j += 1;
		return j;
	}

};

class HeartBeat
{
public:
	MsgHeader header;//0x0002
	HeartBeat()
	{
		header.msgId = 0x0002;
		header.property = 0;
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
		header.msgId = 0x0100;
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
		return j;

	}


};

class Unregister
{
public:
	MsgHeader header; //0x0003

	Unregister()
	{
		header.msgId = 0x0003;
		header.property = 0;
	}
};


class Authentication
{
public:
	MsgHeader header; //0x0102
	STRING code;
	Authentication()
	{
		header.msgId = 0x0102;
	}
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
	WORD sn;
	BYTE result;
	STRING authenticationCode;

	int fromStream(unsigned char* original, int len)
	{
		int j = 0;
		j = header.fromStream(original);
		sn = ntohs(*(WORD*)(original+j));
		j += 2;
		result = *(original+j);
		j += 1;
		if(0 == result)
		{
			authenticationCode = (char*)(original+j);
			j = j + authenticationCode.length() + 1;
		}
		return j;

	}

};


class SetParam
{
	union ParamValue
	{
		DWORD dw;
		WORD w;
		BYTE b;
	};
	struct Param
	{
		DWORD id;
		ParamValue value;
	};
	struct StringParam
	{
		DWORD id;
		STRING value;
	};
public:
	MsgHeader header;//0x8103
	BYTE count;

	SetParam();
	int fromStream(unsigned char *ori)
	{
		int j = 0;
		j = header.fromStream(ori);
		count = *(ori+j);
		j += 1;
		return j;
	}

	int getParam(unsigned char* ori);
	static vector<Param> paramList;
	static vector<StringParam> stringparamlist;
};

class QueryParam
{
public:
	MsgHeader header;//0x8104
};

class QueryParamAck
{
public:
	MsgHeader header;//0x0104
	WORD sn;
	BYTE count;
};

class TerminalControl
{
	enum CMD
	{
		wirelessUpdate = 1,
		connectServer =2,
		terminalShutdown =3,
		terminalReset = 4,
		restoreDefaultSetting =5,
		closeDataCommunication =6,
		closeWirelessCommunication =7
	};
public:
	MsgHeader header;//0x8105
	BYTE cmd;
	STRING param;

	int fromStream(unsigned char * ori, int size);

	BYTE connectControl;
	STRING APN;
	STRING user;
	STRING password;
	STRING address;
	WORD tcpPort;
	WORD udpPort;
	BYTE manufacturerId[5];
	STRING authenticationCode;
	STRING hardwareVersion;
	STRING firmwareVersion;
	STRING URL;
	WORD connectTimeLimit;
};

class PositionReport
{
public:
	MsgHeader header;//0x0200
	DWORD warningMark;
	DWORD status;
	DWORD latitude;
	DWORD longitude;
	WORD altitude;
	WORD speed;
	WORD direction;
	BCD time[6];

	//status
	enum
	{
		acc = 0x00000001,
		location = 0x00000002,
		south  = 0x00000004,
		west = 0x00000008,
		outage = 0x00000010,
		encryption = 0x00000020,
		oilDisconnect = 0x00000400,
		circuitDisconnect = 0x00000800,
		lock =0x00001000,
	};

	//warning mark
	enum
	{
		alarmSwitch = 0x00000001,
		overspeed = 0x00000002,
		fatigueDriving = 0x00000004,
		forewarning = 0x00000008,
		GNSSModuleFailure = 0x00000010,
		GNSSAntennaOffline = 0x00000020,
		GNSSAntennaShortout = 0x00000040,
		undervoltage = 0x00000080,
		powerFailure = 0x00000100,
		displayFault = 0x00000200,
		TTSFailure = 0x00000400,
		cameraFailure = 0x00000800,
		drivingTimeout = 0x00040000,
		parkingTimeout = 0x00080000,
		inOutArea = 0x00100000,
		inOutLine = 0x00200000,
		drivingTimeShortOrTooLong = 0x00400000,
		lineDeparture = 0x00800000,
		VSSFailure = 0x01000000,
		abnormalOil = 0x02000000,
		stolen = 0x04000000,
		illegalIgnition = 0x08000000,
		illegalDisplacement = 0x10000000,
	};


	//additional info id
	enum
	{
		mileage = 0x01,
		oilMass = 0x02,
		recordSpeed = 0x03,
		overspeedAlarm = 0x11,
		inOutAlarm = 0x12,
		timeShortOrTooLong = 0x13,
	};

	struct OverspeedInfo
	{
		BYTE positionType;
		DWORD id;
	};

	struct InOutInfo
	{
		BYTE positionType;
		DWORD id;
		BYTE direction;
	};

	struct ShortOrTooLongInfo
	{
		DWORD id;
		WORD time;
		BYTE result;
	};

	int toStream(unsigned char * original)
	{
		int j;
		j = header.toStream(original);
		DWORD dw = htonl(warningMark);
		memcpy(original+j, &dw, sizeof(dw));
		j += 4;
		dw = htonl(status);
		memcpy(original+j, &dw, sizeof(dw));
		j += 4;
		dw = htonl(latitude);
		memcpy(original+j, &dw, sizeof(dw));
		j += 4;
		dw = htonl(longitude);
		memcpy(original+j, &dw, sizeof(dw));
		j += 4;
		WORD tmp = htons(altitude);
		memcpy(original+j,&tmp, sizeof(tmp));
		j += 2;
		tmp = htons(speed);
		memcpy(original+j,&tmp, sizeof(tmp));
		j += 2;
		tmp = htons(direction);
		memcpy(original+j,&tmp, sizeof(tmp));
		j += 2;
		memcpy(original+j,time,6);
		j += 6;


		return j;
	}

};
#endif /* DATASTRUCT_H_ */
