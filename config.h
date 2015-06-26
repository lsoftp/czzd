#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "datatype.h"
#include <string>
#include <map>






class Config
{
public:
	Config();
	enum ParamId{
		heartBeat = 0x0001,
		tcpTimeout = 0x0002,
		tcpResendTimes = 0x0003,
		udpTimeout = 0x0004,
		udpResendTimes = 0x0005,
		smsTimeout = 0x0006,
		smsResendTimes = 0x0007,

		APN = 0x0010,
		serverUser = 0x0011,
		serverPasswd = 0x0012,
		serverAddress = 0x0013,
		backupAPN = 0x0014,
		backupServerUser = 0x0015,
		backupServerPasswd = 0x0016,
		backupServerAddress = 0x0017,
		tcpPort = 0x0018,
		udpPort = 0x0019,

		positionReportStrategy = 0x0020,
		positionReportScheme = 0x0021,
		notLoginReportInterval = 0x0022,

		sleepReportInterval = 0x0027,
		alarmReportInterval = 0x0028,
		defaultReportInterval = 0x0029,

		defaultDistanceReportInterval = 0x002c,
		notLoginDistanceReportInterval = 0x002d,
		sleepDistanceReportInterval = 0x002e,
		alarmDistanceReportInterval = 0x002f,
		turningPointAngle = 0x0030,

		monitoringPlatformPhoneNumber = 0x0040,
		resetPhoneNumber = 0x0041,
		defaultSettingPhoneNumber = 0x0042,
		monitoringPlatformSMSPhoneNumber = 0x0043,
		alarmTextPhoneNumber = 0x0044,
		terminalPhoneAnswerStrategy = 0x0045,
		maximumCallTime = 0x0046,
		maximumMonthCallTime = 0x0047,
		monitorPhoneNumber = 0x0048,
		regulatoryPlatformPrivilegeSMSNumber = 0x0049,

		alarmMaskDword = 0x0050,
		alarmSMSSwitch = 0x0051,
		alarmFilmSwitch = 0x0052,
		alarmFilmSaveFlag = 0x0053,
		keyPointFlag = 0x0054,
		maximumSpeed = 0x0055,
		overspeedLastTime = 0x0056,
		continuousDrivingTimeThreshold = 0x0057,
		dayTotalDrivingTimeThreshold = 0x0058,
		minimumRestTime = 0x0059,
		maximumParkTime = 0x005a,

		videoQuality = 0x0070,
		brightness = 0x0071,
		contrast = 0x0072,
		saturation = 0x0073,
		chroma = 0x0074,

		odometerReading = 0x0080,
		provinceId = 0x0081,
		cityId = 0x0082,
		plateNumber = 0x0083,
		plateColor = 0x0084
	};




	std::map<DWORD, STRING> m_paramlist;
};
#endif

