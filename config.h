#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "datestruct.h"
#include <string>
#include <map>


using namespace std;



#define COMMENT_CHAR '#'



class Config
{
public:
	Config();
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
	STRING backupServerAddress;
	DWORD tcpPort;
	DWORD udpPort;
	

};
#endif

