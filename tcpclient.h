#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "datastruct.h"
  
class TcpClient  
{  
private:  
        int socket_fd;  
        pthread_t threadHandler;
        int m_timeout; //usecond
        int m_resendtimes; //max re send times
        char message[4096];  
        struct sockaddr_in server_addr;
        static list<Msg> msgList;
        static pthread_mutex_t mutex;
        static list<RecvStream> recvStreamList;
        static pthread_mutex_t mutexRecvStream;
  
public:  
        TcpClient();  
        virtual ~TcpClient();
        static void * sendRecv(void* arg);
		int open(char* server_ip,char* server_port);
		int toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen);
		int toComposedMsg(unsigned char * original,int origlen, unsigned char * composed, int* comlen);
		int addCheckCode(unsigned char * original , int len);
};  


#endif 



