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

#define PROPERTY_LENGTH 0x03ff;
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
        int j , begin = -1,end = -1;
        if(size == 0)
        {
            *len = 0;
            return 0;
        }

        for (j = 0; j < size; j++)
        {
            if (stream[j] == 0x7e)
            {
                if(begin == -1)
                {
                    begin = j;
                }
                else
                {
                    end = j;
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

class PositionQuery
{
public:
	MsgHeader header;//0x8201
};

class PositionQueryAck
{
public:
	MsgHeader header;//0x0201
	WORD sn;
	DWORD warningMark;
	DWORD status;
	DWORD latitude;
	DWORD longitude;
	WORD altitude;
	WORD speed;
	WORD direction;
	BCD time[6];

	int toStream(unsigned char * original)
	{
		int j;
		j = header.toStream(original);
		WORD tmp;
		tmp = htons(sn);
		memcpy(original+j, &tmp, sizeof(tmp));
		j += 2;
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
		tmp= htons(altitude);
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

class TemporaryPositionTrack
{
public:
	MsgHeader header; //0x8202
	WORD interval;
	DWORD validUntil;

	int fromStream(unsigned char * ori);
};

class TextInfo
{
public:
	MsgHeader header; //0x8300
	BYTE mark;
	STRING text;
	enum
	{
		urgent = 0x01,
		display = 0x04,
		TTS = 0x08,
		ad = 0x10,
	};

	int fromStream(unsigned char * ori);
};

class EventSetting
{
public:
	MsgHeader header;//0x8301
	BYTE type;
	BYTE count;

	enum
	{
		deleteEvent = 0x00,
		updateEvent = 0x01,
		appendEvent = 0x02,
		modifyEvent = 0x03,
		deleteSomeEvent = 0x04,

	};

	struct EventItem
	{
		BYTE id;
		BYTE len;
		STRING content;
	};

	int fromStream(unsigned char * ori);
	void handleEvent();
	vector<EventItem> eventFromStream; //
	static vector<EventItem> eventList;
};

class EventReport
{
public:
	MsgHeader header;//0x0301
	BYTE eventId;

	EventReport();
	int toStream(unsigned char * ori);
};

class Ask
{
public:
	MsgHeader header; //0x8302
	BYTE mark;
	BYTE questionLength;
	STRING question;

	enum
	{
		urgent = 0x01,
		tts = 0x08,
		display = 0x10,
	};

	struct Answer
	{
		BYTE id;
		WORD len;
		STRING content;
	};

	int fromStream(unsigned char * ori);
	vector<Answer> answerList;
};

class AskAck
{
public:
	MsgHeader header; //0x0302
	WORD sn;
	BYTE answerId;

	AskAck();

	int toStream(unsigned char * ori);
};

class InfoDemandMenu
{
public:
	MsgHeader header; //0x8303
	BYTE type;
	BYTE count;

	enum
	{
		deleteAllInfo = 0,
		updateMenu = 1,
		appendMenu = 2,
		modifyMenu = 3,
	};

	struct Info
	{
		BYTE type;
		WORD len;
		STRING name;
	};

	vector<Info> infoFromStream;
	static vector<Info> infoList;

	int fromStream(unsigned char * ori);
	void handleInfo();
};

class InfoDemandOrCancel
{
public:
	MsgHeader header; //0x0303
	BYTE type;
	BYTE demandOrCancel;

	enum
	{
		cancel = 0,
		demand = 1
	};

	InfoDemandOrCancel();
	int toStream(unsigned char * ori);

};

class InfoService
{
public:
	MsgHeader header;// 0x8304
	BYTE type;
	WORD len;
	STRING content;

	int fromStream(unsigned char * ori);
};

class Callback
{
public:
	MsgHeader header; //0x8400
	BYTE mark;
	STRING phoneNumber;

	enum
	{
		normal = 0,
		monitor = 1,
	};

	int fromStream(unsigned char * ori);
};

class SetPhoneBook
{
public:
	MsgHeader header; //0x8401
	BYTE type;
	BYTE count;

	enum
	{
		deleteAll = 0,
		update = 1,
		append = 2,
		modify = 3
	};

	enum
	{
		callIn = 1,
		callOut = 2,
		inAndOut = 3,
	};
	struct Contact
	{
		BYTE mark;
		BYTE numberLength;
		STRING phoneNumber;
		BYTE contactLength;
		STRING name;

	};

	vector<Contact> contactFromStream;
	static vector<Contact> contactList;

	int fromStream(unsigned char * ori);
	void handleContact();
};

class VehicleControl
{
public:
	MsgHeader header; //0x8500
	BYTE mark;

	enum
	{
		lock = 0x01
	};

	int fromStream(unsigned char * ori);

};

class VehicleControlAck
{
public:
	MsgHeader header; //0x0500
	WORD sn;
	DWORD warningMark;
	DWORD status;
	DWORD latitude;
	DWORD longitude;
	WORD altitude;
	WORD speed;
	WORD direction;
	BCD time[6];

	int toStream(unsigned char * original);


};

enum AreaProperty
{
	AREA_BY_TIME = 0x01,
	AREA_LIMIT_SPEED = 0x02,
	AREA_IN_TO_DRIVER = 0x04,
	AREA_IN_TO_PLATFORM = 0x08,
	AREA_OUT_TO_DRIVER = 0x10,
	AREA_OUT_TO_PLATFORM = 0x20,
	AREA_SOUTH_LATITUDE = 0x40,
	AREA_WEST_LONGITUDE = 0x80,
};

class SetCircle
{
public:
	MsgHeader header; //0x8600
	BYTE type;
	BYTE count;

	enum
	{
		updateArea = 0,
		appendArea = 1,
		modifyArea = 2,
	};

	struct Circle
	{
		DWORD id;
		WORD property;
		DWORD centerLatitude;
		DWORD centerLongitude;
		DWORD radius;
		BCD startTime[6];
		BCD endTime[6];
		WORD maximumSpeed;
		BYTE overspeedLastTime;
	};

	vector<Circle> circleFromStream;
	static vector<Circle> circleList;

	int fromStream(unsigned char * ori);
	void handleCircle();
};

class DeleteCircle
{
public:
	MsgHeader header; //0x8601
	BYTE count;

	vector<DWORD> idFromStream;

	int fromStream(unsigned char * ori);
	void handleCircle();
};

class SetRect
{
public:
	MsgHeader header; //0x8602
	BYTE type;
	BYTE count;

	enum
	{
		updateArea = 0,
		appendArea = 1,
		modifyArea = 2,
	};

	struct Rect
	{
		DWORD id;
		WORD property;
		DWORD leftTopLatitude;
		DWORD leftTopLongitude;
		DWORD rightBottomLatitude;
		DWORD rightBottomLongitude;
		BCD startTime[6];
		BCD endTime[6];
		WORD maximumSpeed;
		BYTE overspeedLastTime;
	};

	vector<Rect> rectFromStream;
	static vector<Rect> rectList;

	int fromStream(unsigned char * ori);
	void handleRect();
};

class DeleteRect
{
public:
	MsgHeader header; //0x8603
	BYTE count;

	vector<DWORD> idFromStream;

	int fromStream(unsigned char * ori);
	void handleRect();
};

class SetPolygon
{
public:
	MsgHeader header; //0x8604
	DWORD id;
	WORD property;
	BCD startTime[6];
	BCD endTime[6];
	WORD maximumSpeed;
	BYTE overspeedLastTime;
	WORD count;

	struct Point
	{
		DWORD latitude;
		DWORD longitude;
	};

	vector<Point> pointFromStream;

	int fromStream(unsigned char * ori);
	void handlePolygon();

};

class Polygon
{
public:
	static vector<SetPolygon> polygonList;
};

class DeletePolygon
{
public:
	MsgHeader header; //0x8605
	BYTE count;

	vector<DWORD> idFromStream;

	int fromStream(unsigned char * ori);
	void handlePolygon();
};

class SetRoute
{
public:
	MsgHeader header; //0x8606
	DWORD id;
	WORD property;
	BCD startTime[6];
	BCD endTime[6];
	WORD count;

	enum RouteProperty
	{
		ROUTE_BY_TIME = 0x01,
		ROUTE_IN_TO_DRIVER = 0x04,
		ROUTE_IN_TO_PLATFORM = 0x08,
		ROUTE_OUT_TO_DRIVER = 0x10,
		ROUTE_OUT_TO_PLATFORM = 0x20,

	};

	enum LineProperty
	{
		LINE_DRIVING_TIME = 0x01,
		LINE_LIMIT_SPEED = 0x02,
		LINE_SOUTH_LATITUDE = 0x04,
		LINE_WEST_LONGITUDE = 0x08,
	};

	struct TurningPoint
	{
		DWORD turningPointId;
		DWORD lineId;
		DWORD latitude;
		DWORD longitude;
		BYTE lineWidth;
		BYTE lineProperty;
		WORD timeTooLongThreshold;
		WORD timeShortThreshold;
		WORD maximumSpeed;
		WORD overSpeedLastTime;

	};

	vector<TurningPoint> turningPointFromStream;

	int fromStream(unsigned char * ori);
	void handleRoute();
};

class Route
{
public:
	static vector<SetRoute> routeList;
};

class DeleteRoute
{
public:
	MsgHeader header; //0x8607
	BYTE count;

	vector<DWORD> idFromStream;

	int fromStream(unsigned char * ori);
	void handleRoute();
};

class DrivingRecordDataCollect
{
public:
	MsgHeader header; //0x8700
	BYTE cmd;



	int fromStream(unsigned char * ori);

};

class DrivingRecordDataUpload
{
public:
	MsgHeader header; //0x0700
	WORD sn;
	BYTE cmd;

	struct ExecuteVersion
	{
		BCD year;
		BYTE modifyNumer;
	};

	struct DriverInfo
	{
		BYTE driverLicenseNumber[18];
	};

	struct RealTime
	{
		BCD time[6];
	};

	struct TotalMileage
	{
		BCD time[6];
		BCD installTime[6];
		BCD initialMileage[4];
		BCD totalDriveMileage[4];
	};

	struct PulseFactor
	{
		BCD time[6];
		WORD pulse;
	};

	struct VehicleInfo
	{
		BYTE vehicleId[17];
		BYTE plateNumber[12];
		BYTE plateClass[12];
	};

	struct StatusSignalConfig
	{
		BCD time[6];
		BYTE bytes;
		BYTE status[80];

	};

	struct OnlyNo
	{
		BYTE ccc[7];
		BYTE type[16];
		BCD year;
		BCD month;
		BCD day;
		BYTE sn[4];
		BYTE backup[5];
	};

	union Data
	{
		ExecuteVersion executeVersion;
		DriverInfo driverInfo;
		RealTime realTime;
		TotalMileage totalMileage;
		PulseFactor pulseFactor;
		VehicleInfo vehicleInfo;
		StatusSignalConfig statusSignalConfig;
		OnlyNo onlyNo;
	};


};

class DrivingRecordParamDownload
{
public:
	MsgHeader header; //0x8701
	BYTE cmd;

	BYTE * data;
};


class EWaybill
{
public:
	MsgHeader header; //0x0701
	DWORD length;

	BYTE * data;

};

class DriverIdReport
{
public:
	MsgHeader header; //0x0702
	BYTE nameLength;
	STRING name;
	STRING idCardNo; //20
	STRING jobNo; //40
	BYTE CertificateIssuerLength;
	STRING CertificateIssuer;

	int toStream(BYTE * ori);
};

class MultimediaEventInfoUpload
{
public:
	MsgHeader header; //0x0800
	DWORD id;
	BYTE type;
	BYTE format;
	BYTE eventItem;
	BYTE channelId;

	int toStream(BYTE * ori);
};

class MultimediaUpload
{
public:
	MsgHeader header; //0x0801
	DWORD id;
	BYTE type;
	BYTE format;
	BYTE eventItem;
	BYTE channelId;
	BYTE *data;

};

class MultimediaUploadAck
{
public:
	MsgHeader header; //0x8800
	DWORD id;
	BYTE count;

	vector<WORD> idList;

	int fromStream(BYTE * ori);
};

class CameraFilm
{
public:
	MsgHeader header; //0x8801
	BYTE channelId;
	WORD cmd;
	WORD intervalOrTime;
	BYTE saveFlag;
	BYTE resolution;
	BYTE quality;
	BYTE brightness;
	BYTE contrast;
	BYTE saturation;
	BYTE chroma;
	enum
	{
		R320_240 = 0x01,
		R640_480 = 0x02,
		R800_600 = 0x03,
		R1024_768 = 0x04,
		R176_144 = 0x05,
		R352_288 = 0x06,
		R704_288 = 0x07,
		R704_576 = 0x08
	};

	int fromStream(BYTE * ori);
};

class MultimediaDataRetrieve
{
public:
	MsgHeader header; //0x8802
	BYTE type;
	BYTE channelId;
	BYTE eventItem;
	BCD startTime[6];
	BCD endTime[6];

	int fromStream(BYTE * ori);
};

class MultimediaDataRetrieveAck
{
public:
	MsgHeader header; //0x0802
	WORD sn;
	WORD count;

	struct MultimediaDataRetrieveItem
	{
		BYTE type;
		BYTE channelId;
		BYTE eventItem;
		DWORD warningMark;
		DWORD status;
		DWORD latitude;
		DWORD longitude;
		WORD altitude;
		WORD speed;
		WORD direction;
		BCD time[6];
	};

	vector<MultimediaDataRetrieveItem> multimediaList;
	int toStream(BYTE * ori);
};

class MultimediaUploadCmd
{
public:
	MsgHeader header; //0x8803

	BYTE type;
	BYTE channelId;
	BYTE eventItem;
	BCD startTime[6];
	BCD endTime[6];
	BYTE delFlag;

	int fromStream(BYTE * ori);

};


class RecordStartCmd
{
public:
	MsgHeader header; //0x8804
	BYTE cmd;
	WORD time;
	BYTE saveFlag;
	BYTE SampleRate;

	int fromStream(BYTE * ori);

};

class DownPassThrough
{
public:
	MsgHeader header; //0x8900
	BYTE type;
	BYTE *data;
	int size;
};

class UpPassThrough
{
public:
	MsgHeader header; //0x8900
	BYTE type;
	BYTE *data;
	int size;
};

class GzipUpload
{
public:
	MsgHeader header; //0x0901
	DWORD len;
	BYTE *data;
};

class PlatformRsaPublicKey
{
public:
	MsgHeader header; //0x8A00
	DWORD e;
	BYTE n[128];

	int fromStream(BYTE * ori);
};

class TerminalRsaPublicKey
{
public:
	MsgHeader header; //0x0A00
	DWORD e;
	BYTE n[128];

	int toStream(BYTE * ori);
};
#endif /* DATASTRUCT_H_ */
