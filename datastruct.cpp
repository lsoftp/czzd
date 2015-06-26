/*
 * datastruct.cpp
 *
 *  Created on: 2015Äê6ÔÂ26ÈÕ
 *      Author: Administrator
 */


#include "datastruct.h"

vector<SetParam::Param> SetParam::paramList;
vector<SetParam::StringParam> SetParam::stringparamlist;

SetParam::SetParam()
{
	paramList.clear();
	stringparamlist.clear();
}
int SetParam::getParam(unsigned char* ori)
{
	DWORD paramid;
	BYTE length;
	Param pa;
	StringParam sp;
	int j = 0;
	j = fromStream(ori);
	for (int i =0;i<count; i++)
	{
		paramid = ntohl(*(DWORD*)(ori + j));
		j += 4;
		length = *(ori + j);
		j += 1;
		switch(paramid)
		{
			case Config::heartBeat:
			case Config::tcpTimeout:
			case Config::tcpResendTimes:
			case Config::udpTimeout:
			case Config::udpResendTimes:
			case Config::smsTimeout:
			case Config::smsResendTimes:
			case Config::tcpPort:
			case Config::udpPort:
			case Config::positionReportStrategy:
			case Config::positionReportScheme:
			case Config::notLoginReportInterval:
			case Config::sleepReportInterval:
			case Config::alarmReportInterval:
			case Config::defaultReportInterval:
			case Config::defaultDistanceReportInterval:
			case Config::notLoginDistanceReportInterval:
			case Config::sleepDistanceReportInterval:
			case Config::alarmDistanceReportInterval:
			case Config::turningPointAngle:
			case Config::terminalPhoneAnswerStrategy:
			case Config::maximumCallTime:
			case Config::maximumMonthCallTime:
			case Config::alarmMaskDword:
			case Config::alarmSMSSwitch:
			case Config::alarmFilmSwitch:
			case Config::alarmFilmSaveFlag:
			case Config::keyPointFlag:
			case Config::maximumSpeed:
			case Config::overspeedLastTime:
			case Config::continuousDrivingTimeThreshold:
			case Config::dayTotalDrivingTimeThreshold:
			case Config::minimumRestTime:
			case Config::maximumParkTime:
			case Config::videoQuality:
			case Config::brightness:
			case Config::contrast:
			case Config::saturation:
			case Config::chroma:
			case Config::odometerReading:
				pa.id = paramid;
				pa.value.dw = ntohl(*(DWORD*)(ori + j));
				paramList.push_back(pa);

				break;

			case Config::provinceId:
			case Config::cityId:
				pa.id = paramid;
				pa.value.w = ntohs(*(WORD *)(ori + j));
				paramList.push_back(pa);
				break;

			case Config::plateColor:
				pa.id = paramid;
				pa.value.b =*(ori + j);
				paramList.push_back(pa);
				break;

			case Config::APN:
			case Config::serverUser:
			case Config::serverPasswd:
			case Config::serverAddress:
			case Config::backupAPN:
			case Config::backupServerUser:
			case Config::backupServerPasswd:
			case Config::backupServerAddress:
			case Config::monitoringPlatformPhoneNumber:
			case Config::resetPhoneNumber:
			case Config::defaultSettingPhoneNumber:
			case Config::monitoringPlatformSMSPhoneNumber:
			case Config::alarmTextPhoneNumber:
			case Config::monitorPhoneNumber:
			case Config::regulatoryPlatformPrivilegeSMSNumber:
			case Config::plateNumber:

				sp.id = paramid;
			    char * p;
			    p = (char*)(ori + j);
			    int l ;
			    l = strnlen(p,length);
			    if(l != length -1)
			    {
			    	throw length_error("bad string in Set param");
			    }
			    sp.value = p;
			    stringparamlist.push_back(sp);

			    break;

			default:
				break;
		}
		j +=length;
	}
}

int TerminalControl::fromStream(unsigned char * ori, int size)
{
	int j =0;
	j=header.fromStream(ori);
	cmd = *(ori + j);
	j += 1;
	int length = header.property;
	int l;
	switch(cmd)
	{
		case wirelessUpdate:
		case connectServer:
			l = strnlen((char*)(ori + j), length -1);
			if(l != length -2)
			{
				throw length_error("bad string in terminal control");
			}
			param = (char *)(ori+j);
			break;
		default:
			break;
	}
	if(wirelessUpdate == cmd)
	{
		size_t pos = param.find(";");
		if(pos != string::npos)
		{
			URL = param.substr(0,pos);
		}
		size_t pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			APN = param.substr(pos1+1,pos-pos1-1);
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			user = param.substr(pos1+1,pos-pos1-1);
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			password = param.substr(pos1+1,pos-pos1-1);
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			address = param.substr(pos1+1,pos-pos1-1);
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			STRING tp = param.substr(pos1+1,pos-pos1-1);
			tcpPort = atoi(tp.c_str());
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			STRING up = param.substr(pos1+1,pos-pos1-1);
			tcpPort = atoi(up.c_str());
		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			STRING manuid = param.substr(pos1+1,pos-pos1-1);
			if(manuid.length()!= 10)
			{
				throw length_error("bad string of manufaturerid");
			}

			manufacturerId[0] = atoi(manuid.substr(0,1).c_str())<<4|atoi(manuid.substr(1,1).c_str());
			manufacturerId[1] = atoi(manuid.substr(2,1).c_str())<<4|atoi(manuid.substr(3,1).c_str());
			manufacturerId[2] = atoi(manuid.substr(4,1).c_str())<<4|atoi(manuid.substr(5,1).c_str());
			manufacturerId[3] = atoi(manuid.substr(6,1).c_str())<<4|atoi(manuid.substr(7,1).c_str());
			manufacturerId[4] = atoi(manuid.substr(8,1).c_str())<<4|atoi(manuid.substr(9,1).c_str());

		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			hardwareVersion = param.substr(pos1+1,pos-pos1-1);

		}
		pos1 = pos;
		pos = param.find(";",pos1+1);
		if(pos != string::npos)
		{
			firmwareVersion = param.substr(pos1+1,pos-pos1-1);

		}
		pos1 = pos;
		STRING tl=param.substr(pos1+1);
		connectTimeLimit = atoi(tl.c_str());
	}
	if(connectServer == cmd)
	{
		size_t pos = param.find(";");
		if(pos != string::npos)
		{
			STRING cc = param.substr(0,pos);
			connectControl = atoi(cc.c_str());
		}
		else
		{
			connectControl = atoi(param.c_str());
		}
		if(1 != connectControl)
		{
			size_t pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				authenticationCode = param.substr(pos1+1,pos-pos1-1);
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				APN = param.substr(pos1+1,pos-pos1-1);
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				user = param.substr(pos1+1,pos-pos1-1);
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				password = param.substr(pos1+1,pos-pos1-1);
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				address = param.substr(pos1+1,pos-pos1-1);
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				STRING tp = param.substr(pos1+1,pos-pos1-1);
				tcpPort = atoi(tp.c_str());
			}
			pos1 = pos;
			pos = param.find(";",pos1+1);
			if(pos != string::npos)
			{
				STRING up = param.substr(pos1+1,pos-pos1-1);
				tcpPort = atoi(up.c_str());
			}
			pos1 = pos;
			STRING tl=param.substr(pos1+1);
			connectTimeLimit = atoi(tl.c_str());

		}

	}
}
