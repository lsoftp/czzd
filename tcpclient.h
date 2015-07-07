#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "datastruct.h"
  
class TcpClient  
{  

public:
        TcpClient();
        virtual ~TcpClient();
        int start();
        void * sendRecv(void* arg);
        void * handleRecvMsg(void *arg);
        static void * sendRecvMsg(void* arg);
        static void * recvMsg(void* arg);
        void handleRegisterAck(RecvStream *prs);
        void handlePlatformAck(RecvStream *prs);
		int open(char* server_ip,char* server_port);
		int toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen);
		int toComposedMsg(unsigned char * original,int origlen, unsigned char * composed, int* comlen);
		int addCheckCode(unsigned char * original , int len);
		int checkCode(unsigned char * original, int len);
		Msg * getMsgToSend();
		int  handleMsgList();
		void setTime(BCD (&bcd)[6]);
private:  
        int socket_fd;
        pthread_t sendRecvHandler;
        pthread_t recvHandler;



        struct sockaddr_in server_addr;
        list<Msg> msgList;
        pthread_mutex_t mutex;
        list<RecvStream> recvStreamList;
        pthread_mutex_t mutexRecvStream;
        list<WORD> serialNumberList;
        pthread_mutex_t mutexserialNumber;
        pthread_cond_t condRecv;
        WORD m_serialNumber;
        BYTE m_phoneNumber[6];
        int m_timeout; //usecond
        int m_resendtimes; //max re send times
        RecvBuf recvbuf;
};  


#endif 



