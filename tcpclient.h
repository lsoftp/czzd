#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "datastruct.h"
  
class TcpClient  
{  

public:
        TcpClient();
        virtual ~TcpClient();
        int start();
        static void * sendRecv(void* arg);
        static void * handleRecvMsg(void *arg);
        static void handleRegisterAck(RecvStream *prs);
        static void handlePlatformAck(RecvStream *prs);
		static int open(char* server_ip,char* server_port);
		static int toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen);
		static int toComposedMsg(unsigned char * original,int origlen, unsigned char * composed, int* comlen);
		static int addCheckCode(unsigned char * original , int len);
		static int checkCode(unsigned char * original, int len);
		static Msg * getMsgToSend();
		static int  handleMsgList();
		static void setTime(BCD (&bcd)[6]);
private:  
        static int socket_fd;
        pthread_t sendRecvHandler;
        pthread_t recvHandler;



        static struct sockaddr_in server_addr;
        static list<Msg> msgList;
        static pthread_mutex_t mutex;
        static list<RecvStream> recvStreamList;
        static pthread_mutex_t mutexRecvStream;
        static list<WORD> serialNumberList;
        static pthread_mutex_t mutexserialNumber;
        static pthread_cond_t condRecv;
        static WORD m_serialNumber;
        static BYTE m_phoneNumber[6];
        static int m_timeout; //usecond
        static int m_resendtimes; //max re send times
        static RecvBuf recvbuf;
};  


#endif 



