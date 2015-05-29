/*
 * Client.cpp
 *
 *  Created on: 2015年5月29日
 *      Author: Administrator
 */

#include "Client.h"

Client::Client() {
	// TODO Auto-generated constructor stub

}

Client::~Client() {
	// TODO Auto-generated destructor stub
}

void Client::setServer(char* ip, unsigned short port)
{
	strcpy(m_ipaddress,ip);
	m_port = port;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(m_ipaddress);
	server_addr.sin_port = htons(m_port);
	addrlen = sizeof(struct sockaddr_in);
}


int TCPClient::connect()
{
	if( (socket_id = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
	               printf("create socket error: %s(errno:%d)\n)",strerror(errno),errno);
	               return -1;
	}
	if( connect(socket_id,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
	                printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	                return -1;
	}
	return socket_id;

}
int  TCPClient::reconnect()
{
	close(socket_id);
	if( (socket_id = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
	      printf("create socket error: %s(errno:%d)\n)",strerror(errno),errno);
	      return -1;
	}
	if( connect(socket_id,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
	      printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	      return -1;
	}
	return socket_id;
}

int TCPClient::send(const void *buf, int len)
{
	return send(socket_id,buf,len,0);
}

int TCPClient::recv(void *buf, int len)
{
	return recv(socket_id,buf,len,0);
}







int UDPClient::connect()
{
	socket_id = socket(AF_INET,SOCK_DGRAM,0);//创建套接子
	return socket_id;

}
int  UDPClient::reconnect()
{
	close(socket_id);
	socket_id = socket(AF_INET,SOCK_DGRAM,0);
	return socket_id;
}

int UDPClient::send(const void *buf, int len)
{
	return sendto(socket_id,buf,len,0,(struct sockaddr *)&server_addr,addrlen);
}

int UDPClient::recv(void *buf, int len)
{
	return recvfrom(socket_id,buf,len,0,(struct sockaddr *)&server_addr,addrlen);
}
