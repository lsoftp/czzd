/*
 * Client.h
 *
 *  Created on: 2015Äê5ÔÂ29ÈÕ
 *      Author: Administrator
 */

#ifndef CLIENT_H_
#define CLIENT_H_
#include <string.h>

class Client {
public:
	Client();
	virtual ~Client();

	void setServer(char* ip, unsigned short port);
	virtual int connect() = 0;
	virtual int  reconnect()= 0;
	virtual int send(const void *buf, int len) = 0;
	virtual int recv(void *buf, int len) =0;

	char m_ipaddress[16];
	unsigned short m_port;
	int socket_id;
	struct sockaddr_in server_addr;
	int addrlen;
};

class TCPClient:public Client{
public:
	int connect();
	int  reconnect();
	int send(const void *buf, int len);
	int recv(void *buf, int len);
};

class UDPClient:public Client{
public:
	int connect();
	int  reconnect();
	int send(const void *buf, int len);
	int recv(void *buf, int len);

};
#endif /* CLIENT_H_ */
