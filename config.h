#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "datestruct.h"

using namespace std;



class Config
{
private:
 	DWORD heartBeat;
	DWORD tcpTimeout; 
	DWORD tcpResendTimes;
	DWORD udpTimeout;
	DWORD udpResendTimes;
	DWORD smsTimeout;
	DWORD smsResendTimes;
	STRING  APN;
	STRING serverUser;
	STRING serverPasswd;
	STRING serverAddress;
	STRING backupAPN;
	STRING backupServerUser;
	STRING backupServerPasswd;
	STRING backupServerAddresa;
	DWORD tcpPort;
	DWORD udpPort;
	
public:
	Config();
};
#endif

