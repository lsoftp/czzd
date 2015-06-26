#include "config.h"


Config::Config()
{
	m_paramlist[heartBeat] = "5";
	m_paramlist[tcpTimeout] = "5";
	m_paramlist[tcpResendTimes] = "3";
	m_paramlist[udpTimeout] = "5";
	m_paramlist[udpResendTimes] = "3";
	m_paramlist[smsTimeout] = "5";
	m_paramlist[smsResendTimes] = "3";

	m_paramlist[APN] = "cmnet";
	m_paramlist[serverUser] = "";
	m_paramlist[serverPasswd] = "";
	m_paramlist[serverAddress] = "127.0.0.1";
	m_paramlist[backupAPN] = "cmnet";
	m_paramlist[backupServerUser] = "";
	m_paramlist[backupServerPasswd] = "";
	m_paramlist[backupServerAddress] = "127.0.0.1";
	m_paramlist[tcpPort] = "8899";
	m_paramlist[udpPort] = "8888";

	m_paramlist[positionReportStrategy] = "0";//0 for by time interval, 1 by distance 2 by time and distance
	m_paramlist[positionReportScheme] = "0";//0 by acc, 1 by login and acc
	m_paramlist[notLoginReportInterval] = "6";

	m_paramlist[sleepReportInterval] = "6" ;
	m_paramlist[alarmReportInterval] =  "6" ;
	m_paramlist[defaultReportInterval] =  "6" ;

	m_paramlist[defaultDistanceReportInterval] =  "6";
	m_paramlist[notLoginDistanceReportInterval] =  "6";
	m_paramlist[sleepDistanceReportInterval] =  "6";
	m_paramlist[alarmDistanceReportInterval] =  "6";
	m_paramlist[turningPointAngle] =  "30";

	m_paramlist[monitoringPlatformPhoneNumber] = "13971452512";
	m_paramlist[resetPhoneNumber] = "13945652145";
	m_paramlist[defaultSettingPhoneNumber] = "13945652146";
	m_paramlist[monitoringPlatformSMSPhoneNumber] = "13971452513";
	m_paramlist[alarmTextPhoneNumber] = "13971452514";
	m_paramlist[terminalPhoneAnswerStrategy] = "0";
	m_paramlist[maximumCallTime] = "100";
	m_paramlist[maximumMonthCallTime] = "100";

	m_paramlist[monitorPhoneNumber] = "13971452515";
	m_paramlist[regulatoryPlatformPrivilegeSMSNumber] = "13971452516";

	m_paramlist[alarmMaskDword] = "100";
	m_paramlist[alarmSMSSwitch] = "100";
	m_paramlist[alarmFilmSwitch] = "100";
	m_paramlist[alarmFilmSaveFlag] = "100";
	m_paramlist[keyPointFlag] = "100";
	m_paramlist[maximumSpeed] = "100";
	m_paramlist[overspeedLastTime] =  "100";
	m_paramlist[continuousDrivingTimeThreshold] =  "100";
	m_paramlist[dayTotalDrivingTimeThreshold] =  "100";
	m_paramlist[minimumRestTime] =  "100";
	m_paramlist[maximumParkTime] = "100";

	m_paramlist[videoQuality] = "1";//1-10
	m_paramlist[brightness] = "1";//0-255
	m_paramlist[contrast] = "1";//0-127
	m_paramlist[saturation] = "1";//0-127
	m_paramlist[chroma] = "1";//0-255

	m_paramlist[odometerReading] = "1";
	m_paramlist[provinceId] = "1";
	m_paramlist[cityId] = "1";
	m_paramlist[plateNumber] = "1";
	m_paramlist[plateColor] = "1";



}
