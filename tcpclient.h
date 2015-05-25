#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <unistd.h>  
#include <iostream>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <errno.h>  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
  
class TcpClient  
{  
private:  
        int socket_fd;  
        char message[4096];  
        struct sockaddr_in server_addr;  
  
public:  
        TcpClient();  
		int open(char* server_ip,char* server_port);
		int toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen);
		int toComposedMSg(unsigned char * original,int origlen, unsigned char * composed, int* comlen);
		int addCheckCode(unsigned char * original , int len);
};  


#endif 



