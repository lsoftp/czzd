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
#include <sys/types.h>
#include <sys/times.h>
#include <sys/select.h>
#include "pthread.h"

  
class TcpClient  
{  
private:  
        int socket_fd;  
        pthread_t threadHandler;
        char message[4096];  
        struct sockaddr_in server_addr;  
  
public:  
        TcpClient();  
        virtual ~TcpClient();
        static void * threadFunc(void* arg);
		int open(char* server_ip,char* server_port);
		int toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen);
		int toComposedMSg(unsigned char * original,int origlen, unsigned char * composed, int* comlen);
		int addCheckCode(unsigned char * original , int len);
};  


#endif 



