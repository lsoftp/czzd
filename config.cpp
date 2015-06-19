#include "config.h"
Param::Param(DWORD i,string s):id(i),value(s)
{

}

Config::Config()
{
	m_paramlist["heartBeat"] = Param(0x0001,"5");
	m_paramlist["tcpTimeout"] = Param(0x0002,"5");
	m_paramlist["tcpResendTimes"] = Param(0x0003,"3");
	m_paramlist["udpTimeout"] = Param(0x0004,"5");
	m_paramlist["udpResendTimes"] = Param(0x0005,"3");
	m_paramlist["smsTimeout"] = Param(0x0006,"5");
	m_paramlist["smsResendTimes"] = Param(0x0007,"3");

	m_paramlist["APN"] = Param(0x0010,"cmnet");
	m_paramlist["serverUser"] = Param(0x0011,"");
	m_paramlist["serverPasswd"] = Param(0x0012,"");
	m_paramlist["serverAddress"] = Param(0x0013,"127.0.0.1");
	m_paramlist["backupAPN"] = Param(0x0014,"cmnet");
	m_paramlist["backupServerUser"] = Param(0x0015,"");
	m_paramlist["backupServerPasswd"] = Param(0x0016,"");
	m_paramlist["backupServerAddress"] = Param(0x0017,"127.0.0.1");
	m_paramlist["tcpPort"] = Param(0x0018,"8899");
	m_paramlist["udpPort"] = Param(0x0019,"8888");

	m_paramlist["positionReportStrategy"] = Param(0x0020,"0");//0 for by time interval, 1 by distance 2 by time and distance
	m_paramlist["positionReportScheme"] = Param(0x0021,"0");//0 by acc, 1 by login and acc
	m_paramlist["notLoginReportInterval"] = Param(0x0022,"6");

	m_paramlist["sleepReportInterval"] = Param(0x0027,"6");
	m_paramlist["alarmReportInterval"] = Param(0x0028,"6");
	m_paramlist["defaultReportInterval"] = Param(0x0029,"6");





}
