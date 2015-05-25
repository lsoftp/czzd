#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <string>

using namespace std;

typedef string STRING;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char BCD;

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

