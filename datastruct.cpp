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
	return j+l+1;
}


int TemporaryPositionTrack::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	interval = ntohs(*(WORD *)(ori + j ));
	j += 2;
	validUntil = ntohl(*(DWORD *)(ori + j));
	j += 4;
	return j;

}


int TextInfo::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	mark = *(ori + j);
	j += 1;
	int l = header.property & PROPERTY_LENGTH;
	char *p = (char *)(ori + j);
	int len = strnlen(p, l-1);
	if(len != l-2)
	{
		throw length_error("bad string in text info");
	}
	text = p;

}

vector<EventSetting::EventItem> EventSetting::eventList;

int EventSetting::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	if(type != deleteEvent)
	{
		count = *(ori + j);
		j += 1;

		for(int i = 0; i < count; i++)
		{
			EventItem ei;
			ei.id = *(ori + j);
			j += 1;
			ei.len = *(ori + j);
			j += 1;
			int l = ei.len;
			int len;
			char * p = (char*)(ori + j);
			if(l != 0)
			{
				len = strnlen(p, l);
				if(len != l -1)
				{
					throw length_error("bad string in event setting");
				}
				else
				{
					ei.content = p;
				}
			}
			eventFromStream.push_back(ei);
			j += l;
		}
	}
}

void EventSetting::handleEvent()
{
	switch(type)
	{
		case deleteEvent: 	eventList.clear();
							break;
		case updateEvent: 	eventList.swap(eventFromStream);
							break;
		case appendEvent:	for(int i =0; i < eventFromStream.size(); i++)
							{
								eventList.push_back(eventFromStream[i]);
							}
							break;
		case modifyEvent:	for(int i = 0; i < eventFromStream.size(); i++)
							{
								EventItem ei = eventFromStream[i];
								for(int j = 0; j < eventList.size(); j++)
								{
									if(ei.id == eventList[j].id)
									{
										eventList[j].len = ei.len;
										eventList[j].content = ei.content;
									}
								}
							}
							break;
		case deleteSomeEvent:for(int i = 0; i < eventFromStream.size(); i++)
							{
								EventItem ei = eventFromStream[i];
								vector<EventItem>::iterator it=eventList.begin();
								for(; it != eventList.end(); )
								{
									if(ei.id == (*it).id)
									{
										it  = eventList.erase(it);

									}
									else
									{
										it++;
									}
								}
							}
							break;
		default:
			break;
	}




}

EventReport::EventReport()
{
	header.msgId = 0x0301;
	header.property = 1;
}

int EventReport::toStream(unsigned char * ori)
{
	int j = 0;
	j = header.toStream(ori);
	*(ori + j) = eventId;
	j += 1;

	return j;
}

int Ask::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	int l = header.property & PROPERTY_LENGTH;
	mark = *(ori + j);
	j += 1;
	questionLength = *(ori + j);
	j += 1;
	char * p = (char *)(ori + j);
	int len = strnlen(p, questionLength);
	if(len != questionLength -1)
	{
		throw length_error("bad string in ask question");
	}
	question = p;
	j += questionLength;
	l = l - questionLength -2;
	while(l >= 3)
	{
		Answer an;
		an.id = *(ori + j);
		j += 1;
		an.len = ntohs(*(WORD *)(ori + j));
		j += 2;
		p = (char *)(ori + j);
		len = strnlen(p, an.len);
		if(len != an.len -1)
		{
			throw length_error("bad string in ask answer");
		}
		an.content = p;
		j += an.len;
		l = l - an.len -3;
		answerList.push_back(an);
	}



}

AskAck::AskAck()
{
	header.msgId = 0x0302;
	header.property = 3;
}

int AskAck::toStream(unsigned char *ori)
{
	int j = 0;
	j = header.toStream(ori);
	WORD tmp;
	tmp = htons(sn);
	memcpy(ori + j, &tmp, 2);
	j += 2;

	*(ori + j) = answerId;
	j += 1;

	return j;
}

vector<InfoDemandMenu::Info> InfoDemandMenu::infoList;

int InfoDemandMenu::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	count = *(ori + j);
	j += 1;

	for (int i = 0; i < count ; i++)
	{
		Info in;
		in.type = *(ori + j);
		j += 1;
		in.len = ntohs(*(WORD *)(ori + j));
		j += 2;
		char *p=(char *)(ori + j);
		int len = strnlen(p, in.len);
		if( len != in.len - 1)
		{
			throw length_error("bad string in info menu");
		}
		in.name = p;
		j = j + in.len;
		infoFromStream.push_back(in);
	}
}


void InfoDemandMenu::handleInfo()
{
	switch(type)
	{
		case deleteAllInfo: infoList.clear();
							break;
		case updateMenu:	infoList.swap(infoFromStream);
							break;
		case appendMenu:	for (int i = 0; i< infoFromStream.size(); i++)
							{
								infoList.push_back(infoFromStream[i]);

							}
							break;
		case modifyMenu:	for (int i = 0; i< infoFromStream.size(); i++)
							{
								Info in = infoFromStream[i];

								for(int j = 0; j < infoList.size(); j++)
								{
									if(in.type == infoList[j].type)
									{
										infoList[j].len = in.len;
										infoList[j].name = in.name;
									}
								}
							}
							break;
		default:
			break;
	}
}


InfoDemandOrCancel::InfoDemandOrCancel()
{
	header.msgId = 0x0303;
	header.property = 2;
}

int InfoDemandOrCancel::toStream(unsigned char * ori)
{
	int j = 0;
	j = header.toStream(ori);
	*(ori + j) = type;
	j += 1;
	*(ori + j) = demandOrCancel;
	j += 1;

	return j;

}


int InfoService::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	len = ntohs(*(WORD *)(ori + j));
	j += 2;
	char *p = (char *)(ori +j);
	int l = strnlen(p, len);
	if(l != len - 1)
	{
		throw length_error("bad string in infoService");
	}
	content = p;
	j += len;

	return j;
}

int Callback::fromStream(unsigned char *ori)
{
	int j = 0;
	j = header.fromStream(ori);
	mark = *(ori + j);
	j += 1;
	int l = header.property & PROPERTY_LENGTH;
	char *p = (char *)(ori + j);
	int len = strnlen(p, l-1);
	if(len != l - 2)
	{
		throw length_error("bad string in callback");
	}
	phoneNumber = p;
	j = j + len + 1;

	return j;
}

vector<SetPhoneBook::Contact> SetPhoneBook::contactList;

int SetPhoneBook::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	count = *(ori + j);
	j += 1;

	for(int i = 0; i < count; i++)
	{
		Contact co;
		co.mark = *(ori + j);
		j += 1;
		co.numberLength = *(ori + j);
		j += 1;
		char *p =(char *)(ori + j);
		int len = strnlen(p, co.numberLength);
		if(len != co.numberLength -1)
		{
			throw length_error("bad string in number set phone book");
		}
		co.phoneNumber = p;
		j += co.numberLength;

		co.contactLength = *(ori + j);
		j += 1;
		p =(char *)(ori + j);
		len = strnlen(p, co.contactLength);
		if(len != co.contactLength -1)
		{
			throw length_error("bad string in contact set phone book");
		}
		co.name = p;
		j += co.contactLength;
		contactFromStream.push_back(co);
	}

	return j;
}

void SetPhoneBook::handleContact()
{
	switch(type)
	{
		case deleteAll:	contactList.clear();
						break;
		case update:	contactList.swap(contactFromStream);
						break;
		case append:	for(int i = 0; i < contactFromStream.size(); i++ )
						{
							contactList.push_back(contactFromStream[i]);
						}
						break;
		case modify:	for(int i = 0; i < contactFromStream.size(); i++ )
						{
							Contact co = contactFromStream[i];
							for(int j = 0; j < contactList.size(); j++)
							{
								Contact &co1 = contactList[j];
								if(co1.name.compare(co.name) == 0)
								{
									co1.mark = co.mark;
									co1.numberLength = co.numberLength;
									co1.phoneNumber = co.phoneNumber;

								}
							}
						}
						break;
		default:
			break;
	}
}

int VehicleControl::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	mark = *(ori + j);
	j += 1;

	return j;
}

int VehicleControlAck::toStream(unsigned char * original)
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

vector<SetCircle::Circle> SetCircle::circleList;

int SetCircle::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	count = *(ori + j);
	j += 1;

	for(int i = 0; i < count; i++)
	{
		Circle ci;
		ci.id = ntohl(*(DWORD *)(ori + j));
		j += 4;
		ci.property = ntohs(*(WORD *)(ori + j));
		j += 2;
		ci.centerLatitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		ci.centerLongitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		ci.radius = ntohl(*(DWORD*)(ori + j));
		j += 4;
		if(ci.property & AREA_BY_TIME)
		{
			memcpy(&ci.startTime, ori + j, 6);
			j += 6;
			memcpy(&ci.endTime, ori + j, 6);
			j += 6;
		}
		if(ci.property & AREA_LIMIT_SPEED)
		{
			ci.maximumSpeed = ntohs(*(WORD *)(ori + j));
			j += 2;
			ci.overspeedLastTime = *(ori + j);
			j += 1;
		}
		circleFromStream.push_back(ci);
	}
}

void SetCircle::handleCircle()
{
	switch(type)
	{
		case updateArea:
			circleList.swap(circleFromStream);
			break;
		case appendArea:
			for(int i = 0; i < circleFromStream.size(); i++)
			{
				circleList.push_back(circleFromStream[i]);
			}
			break;
		case modifyArea:
			for(int i = 0; i < circleFromStream.size(); i++)
			{
				Circle ci = circleFromStream[i];
				for(int j = 0; j < circleList.size(); j++)
				{
					if(circleList[j].id == ci.id)
					{
						circleList[j] = ci;
					}
				}
			}
			break;
		default:
			break;
	}

}

int DeleteCircle::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	count = *(ori + j);
	for(int i = 0; i < count; i++)
	{
		DWORD dw = ntohl(*(DWORD*)(ori+j));
		j += 4;
		idFromStream.push_back(dw);
	}

	return j;
}

void DeleteCircle::handleCircle()
{
	if(0 == count)
	{
		SetCircle::circleList.clear();
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			vector<SetCircle::Circle>::iterator it=SetCircle::circleList.begin();
			for(; it != SetCircle::circleList.end(); )
			{
				if(idFromStream[i] == (*it).id)
				{
					it  = SetCircle::circleList.erase(it);

				}
				else
				{
					it++;
				}
			}
		}
	}
}


vector<SetRect::Rect> SetRect::rectList;

int SetRect::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	type = *(ori + j);
	j += 1;
	count = *(ori + j);
	j += 1;

	for(int i = 0; i < count; i++)
	{
		Rect re;
		re.id = ntohl(*(DWORD *)(ori + j));
		j += 4;
		re.property = ntohs(*(WORD *)(ori + j));
		j += 2;
		re.leftTopLatitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		re.leftTopLongitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		re.rightBottomLatitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		re.rightBottomLongitude = ntohl(*(DWORD*)(ori + j));
		j += 4;
		if(re.property & AREA_BY_TIME)
		{
			memcpy(&re.startTime, ori + j, 6);
			j += 6;
			memcpy(&re.endTime, ori + j, 6);
			j += 6;
		}
		if(re.property & AREA_LIMIT_SPEED)
		{
			re.maximumSpeed = ntohs(*(WORD *)(ori + j));
			j += 2;
			re.overspeedLastTime = *(ori + j);
			j += 1;
		}
		rectFromStream.push_back(re);
	}
}

void SetRect::handleRect()
{
	switch(type)
	{
		case updateArea:
			rectList.swap(rectFromStream);
			break;
		case appendArea:
			for(int i = 0; i < rectFromStream.size(); i++)
			{
				rectList.push_back(rectFromStream[i]);
			}
			break;
		case modifyArea:
			for(int i = 0; i < rectFromStream.size(); i++)
			{
				Rect re = rectFromStream[i];
				for(int j = 0; j < rectList.size(); j++)
				{
					if(rectList[j].id == re.id)
					{
						rectList[j] = re;
					}
				}
			}
			break;
		default:
			break;
	}

}

int DeleteRect::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	count = *(ori + j);
	for(int i = 0; i < count; i++)
	{
		DWORD dw = ntohl(*(DWORD*)(ori+j));
		j += 4;
		idFromStream.push_back(dw);
	}

	return j;
}

void DeleteRect::handleRect()
{
	if(0 == count)
	{
		SetRect::rectList.clear();
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			vector<SetRect::Rect>::iterator it=SetRect::rectList.begin();
			for(; it != SetRect::rectList.end(); )
			{
				if(idFromStream[i] == (*it).id)
				{
					it  = SetRect::rectList.erase(it);

				}
				else
				{
					it++;
				}
			}
		}
	}
}


int SetPolygon::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	id = ntohl(*(DWORD *)(ori + j));
	j += 4;
	property = ntohs(*(WORD *)(ori + j));
	j += 2;
	if(property & AREA_BY_TIME)
	{
		memcpy(&startTime, ori + j, 6);
		j += 6;
		memcpy(&endTime, ori + j, 6);
		j += 6;
	}
	if(property & AREA_LIMIT_SPEED)
	{
		maximumSpeed = ntohs(*(WORD *)(ori + j));
		j += 2;
		overspeedLastTime = *(ori + j);
		j += 1;
	}
	count = ntohl(*(WORD *)(ori + j));
	j += 2;

	for(int i = 0; i < count; i++)
	{
		Point p;
		p.latitude = ntohl(*(DWORD *)(ori + j));
		j += 4;
		p.longitude =  ntohl(*(DWORD *)(ori + j));
		j += 4;
		pointFromStream.push_back(p);
	}

	return j;
}

void SetPolygon::handlePolygon()
{

	Polygon::polygonList.push_back(*(this));
}

vector<SetPolygon> Polygon::polygonList;

int DeletePolygon::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	count = *(ori + j);
	for(int i = 0; i < count; i++)
	{
		DWORD dw = ntohl(*(DWORD*)(ori+j));
		j += 4;
		idFromStream.push_back(dw);
	}

	return j;
}

void DeletePolygon::handlePolygon()
{
	if(0 == count)
	{
		Polygon::polygonList.clear();
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			vector<SetPolygon>::iterator it=Polygon::polygonList.begin();
			for(; it != Polygon::polygonList.end(); )
			{
				if(idFromStream[i] == (*it).id)
				{
					it  = Polygon::polygonList.erase(it);

				}
				else
				{
					it++;
				}
			}
		}
	}
}

int SetRoute::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	id = ntohl(*(DWORD *)(ori + j));
	j += 4;
	property = ntohs(*(WORD *)(ori + j));
	j += 2;
	if(property & ROUTE_BY_TIME)
	{
		memcpy(&startTime, ori + j, 6);
		j += 6;
		memcpy(&endTime, ori + j, 6);
		j += 6;
	}
	count = ntohs(*(WORD *)(ori + j));

	for(int i = 0; i < count; i++)
	{
		TurningPoint tp;
		tp.turningPointId = ntohl(*(DWORD *)(ori + j));
		j += 4;
		tp.lineId = ntohl(*(DWORD *)(ori + j));
		j += 4;
		tp.latitude = ntohl(*(DWORD *)(ori + j));
		j += 4;
		tp.longitude = ntohl(*(DWORD *)(ori + j));
		j += 4;
		tp.lineWidth = *(ori + j);
		j += 1;
		tp.lineProperty = *(ori + j);
		j += 1;
		if(tp.lineProperty & LINE_DRIVING_TIME)
		{
			tp.timeTooLongThreshold = ntohs(*(WORD *)(ori + j));
			j += 2;
			tp.timeShortThreshold = ntohs(*(WORD *)(ori + j));
			j += 2;
		}
		if(tp.lineProperty & LINE_LIMIT_SPEED)
		{
			tp.maximumSpeed = ntohs(*(WORD *)(ori + j));
			j += 2;
			tp.overSpeedLastTime = ntohs(*(WORD *)(ori + j));
			j += 2;
		}
		turningPointFromStream.push_back(tp);
	}
}

void SetRoute::handleRoute()
{
	Route::routeList.push_back(*this);
}

vector<SetRoute> Route::routeList;

int DeleteRoute::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	count = *(ori + j);
	for(int i = 0; i < count; i++)
	{
		DWORD dw = ntohl(*(DWORD*)(ori+j));
		j += 4;
		idFromStream.push_back(dw);
	}

	return j;
}

void DeleteRoute::handleRoute()
{
	if(0 == count)
	{
		Route::routeList.clear();
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			vector<SetRoute>::iterator it=Route::routeList.begin();
			for(; it != Route::routeList.end(); )
			{
				if(idFromStream[i] == (*it).id)
				{
					it  = Route::routeList.erase(it);

				}
				else
				{
					it++;
				}
			}
		}
	}
}

int DrivingRecordDataCollect::fromStream(unsigned char * ori)
{
	int j = 0;
	j = header.fromStream(ori);
	cmd = *(ori + j);
	j += 1;

	return j;
}
