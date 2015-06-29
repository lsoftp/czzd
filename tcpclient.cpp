
#include "tcpclient.h"
#include "datastruct.h"

list<Msg> TcpClient::msgList;
pthread_mutex_t TcpClient::mutex;
list<RecvStream> TcpClient::recvStreamList;
pthread_mutex_t TcpClient::mutexRecvStream;
list<WORD> TcpClient::serialNumberList;
pthread_mutex_t TcpClient::mutexserialNumber;
pthread_cond_t  TcpClient::condRecv = PTHREAD_COND_INITIALIZER;
WORD TcpClient::m_serialNumber = 0;
BYTE TcpClient::m_phoneNumber[6] = {0x01,0x39,0x16,0x85,0x96,0x14};
int TcpClient::m_timeout = 2500000;
int TcpClient::m_resendtimes = 3;
int TcpClient::socket_fd;
struct sockaddr_in TcpClient::server_addr;
RecvBuf TcpClient::recvbuf;

TcpClient::TcpClient()
{  
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&mutexRecvStream,NULL);
	pthread_mutex_init(&mutexserialNumber,NULL);

}  

TcpClient::~TcpClient()
{
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutexRecvStream);
	pthread_mutex_destroy(&mutexserialNumber);
	pthread_cond_destroy(&condRecv);

}

void * TcpClient::sendRecv(void* arg)
{
		pthread_detach(pthread_self());
	   TcpClient *ptc = (TcpClient*)arg;
	   int ret1 =open("127.0.0.1","8899");
	   if(ret1 != 0) return NULL;
	   int sfd=ptc->socket_fd;
	   fd_set readset, writeset;
	   Msg *pmsg=NULL;

	   RecvStream recvstream;
	   struct timeval timeout={0,200},endtime;
	   int maxfd;
	   int ret, sendChars,recvChars, r;

	   while (1)
	   {

		 handleMsgList();


		 if(pmsg == NULL)
		 {
			 pmsg = getMsgToSend();
		 }


	     FD_ZERO(&readset);            //每次循环都要清空集合，否则不能检测描述符变化
	     FD_SET(sfd, &readset);     //添加描述符
	     FD_ZERO(&writeset);
	     FD_SET(sfd,&writeset);

	     maxfd = sfd +1;    //描述符最大值加1


	    ret = select(maxfd, &readset, &writeset, NULL, &timeout);   // 阻塞模式
	     switch( ret)
	     {
	       case -1:
	         exit(-1);
	         break;
	       case 0:  //timeout

	         break;
	       default:

	         if (FD_ISSET(sfd, &readset))  //测试sock是否可读，即是否网络上有数据
	         {
	        	 printf("recvstream len:%d\n", recvbuf.size);
	    	   	 fflush(stdout);
	    	   	 pthread_mutex_lock(&mutexRecvStream);
	        	 recvChars = recv(sfd, recvbuf.stream+recvbuf.size, 1560-recvbuf.size,0);

	        	 if( recvChars < 0 )
	        	 {
	        	     printf("recv message error\n");
	        	 	 return NULL;
	        	 }
	        	 else if(recvChars == 0)
	        	 {
	        		 //网络断开，尝试重连
	        	 }
	        	 else{
	        		 recvbuf.size += recvChars;


	        	 }
	        	 if(recvbuf.size > 0)
	        	 {
	        		 pthread_cond_signal(&condRecv);
	        	 }
	        	 pthread_mutex_unlock(&mutexRecvStream);
	         }
	          if (FD_ISSET(sfd, &writeset))
	         {
	        	  if((pmsg != NULL) && (pmsg->sendChars < pmsg->len)&&(!pmsg->complete))
	        	  {
	        		  sendChars = send( sfd,pmsg->stream+pmsg->sendChars, pmsg->len - pmsg->sendChars,0 );
	        		  if( sendChars < 0 )
	        		  {
	        	                  printf("send message error\n");
	        	                  return NULL;
	        		  }
	        		  else
	        		  {
	        			  pmsg->sendChars += sendChars;
	        		  }
	        	  }
	        	  if((pmsg != NULL) && (pmsg->sendChars == pmsg->len)&&(!pmsg->complete))
	        	  {
	        		  (pmsg->resendTimes)++;
	        		  gettimeofday(&(pmsg->sendTime),NULL);
	        		  pmsg->complete = true;
	        		  pmsg = NULL;
	        	  }
	         }

         }
     }


	//pthread_detach(pthread_self());
	return NULL;
}

void * TcpClient::handleRecvMsg(void *arg)
{
	pthread_detach(pthread_self());
	TcpClient *ptc = (TcpClient*)arg;
	RecvStream recvstream,rs;
	int i,r = 0;

	while(1)
	{
		pthread_mutex_lock(&mutexRecvStream);
		if(recvbuf.size <=0)
		{
			pthread_cond_wait(&condRecv,&mutexRecvStream);
		}
		r = recvbuf.getDataFromBuf(recvstream.stream, &(recvstream.size));
		pthread_mutex_unlock(&mutexRecvStream);
		 if(r)
		 {
			 for(int t=0; t<recvstream.size;t++)
			                 printf("%02x ", recvstream.stream[t]);
			             printf("\n");
			 ptc->toOriginalMsg(recvstream.stream, recvstream.size, rs.stream, &rs.size);
			i = ptc->checkCode(rs.stream, rs.size);
			if(i<0)
			{
				continue;
			}
			if(i==1)
			{
			//parse msg
				MsgHeader header;
				int j = header.fromStream(rs.stream);
				switch(header.msgId)
				{
					case 0x8100: handleRegisterAck(&rs);break;
					case 0x8001: handlePlatformAck(&rs);break;
					default:break;
				}
			}
			r=0;
		 }
	}

}

void TcpClient::handlePlatformAck(RecvStream *prs)
{
	PlatformAck pa;
	int j= pa.fromStream(prs->stream);
	WORD serialNumber = pa.serialNumber;
	cout<<"sn:"<<pa.serialNumber<<endl;
	pthread_mutex_lock(&mutexserialNumber);
	serialNumberList.push_front(serialNumber);
	pthread_mutex_unlock(&mutexserialNumber);

}
void TcpClient::handleRegisterAck(RecvStream *prs)
{
	RegisterAck ra;
	int j = ra.fromStream(prs->stream, prs->size);
	WORD serialNumber = ra.sn;
	cout<<"sn:"<<ra.sn<<endl;
	pthread_mutex_lock(&mutexserialNumber);
	serialNumberList.push_front(serialNumber);
	pthread_mutex_unlock(&mutexserialNumber);

	if(ra.result == 0)
	{
		Authentication au;
		au.code = ra.authenticationCode;
		au.header.property = 12+au.code.length() +1;
		memcpy(au.header.phoneNumber, m_phoneNumber,6);
		Msg msg;
		unsigned char ori[1024];
		int len;
		len = au.toStream(ori);
		len = addCheckCode(ori,len);

		toComposedMsg(ori,len, msg.stream, &(msg.len));
		msg.resendTimes=0;
		msg.msgSerialNumber = au.header.msgSerialNumber;
		msg.sendChars = 0;
		msg.complete = false;
		pthread_mutex_lock(&mutex);
		msgList.push_front(msg);
		pthread_mutex_unlock(&mutex);

	}
}

Msg * TcpClient::getMsgToSend()
{
	Msg *pmsg=NULL, *pmsg1 = NULL;
	struct timeval endtime;
	gettimeofday(&endtime, NULL);
	list<Msg>::iterator it;
//	printf("sdfdfdsfdsf\n");
	//fflush(stdout);
	pthread_mutex_lock(&mutex);
	for(it=msgList.begin(); it != msgList.end(); ++it)
	{

		pmsg = &(*it);
		long int time = (endtime.tv_sec - pmsg->sendTime.tv_sec)*1000000 + (endtime.tv_usec - pmsg->sendTime.tv_usec);
		if(!(pmsg->complete))
		{
			pmsg1 = pmsg;
			break;
		}
		else if((time >= m_timeout) &&!(pmsg->isAck))
		{
			if(pmsg->resendTimes <m_resendtimes)
			{
				pmsg->sendChars = 0;
				pmsg->complete = false;
				pmsg1 = pmsg;
				break;
			}

		}
	}
	pthread_mutex_unlock(&mutex);
	return pmsg1;
}
int  TcpClient::handleMsgList()
{
	WORD sn;
	Msg *pmsg = NULL;
	list<WORD>::iterator it;
	pthread_mutex_lock(&mutexserialNumber);
	for(it = serialNumberList.begin(); it != serialNumberList.end(); )
	{
		sn = *it;
		list<Msg>::iterator it1;
		pthread_mutex_lock(&mutex);
		for(it1 = msgList.begin(); it1 != msgList.end(); )
		{
			pmsg = &(*it1);


			if((pmsg->complete)&&((pmsg->msgSerialNumber == sn) ||(pmsg->resendTimes >= m_resendtimes)||(pmsg->isAck)))
			{
				msgList.erase(it1++);
				if(pmsg->resendTimes >= m_resendtimes)
				{
					//save msg or other things
				}

			}
			else
			{
				++it1;
			}
		}
		pthread_mutex_unlock(&mutex);
		serialNumberList.erase(it++);
	}
	pthread_mutex_unlock(&mutexserialNumber);
}
int TcpClient::start()
{
    int ret = pthread_create(&sendRecvHandler, NULL, TcpClient::sendRecv, this);
    if(ret != 0)
    {
        return ret;
    }
    ret = pthread_create(&recvHandler, NULL, TcpClient::handleRecvMsg, this);
    if(ret != 0)
    {
        return ret;
    }
    unsigned char s[6] = {0x01,0x39,0x34,0x34,0x36,0x44};
    unsigned char ori[1024];
    PositionReport pr;
    setTime(pr.time);
    pr.header.msgId = 0x0200;
    pr.header.property = 28;
    pr.header.msgSerialNumber =222;
    memcpy(pr.header.phoneNumber ,s ,6);
    pr.speed = 18;
    pr.latitude =333000;
    pr.longitude = 567890;
    pr.altitude = 300;
    pr.direction=256;
    pr.status =0x7897;
    pr.warningMark =0;
    Msg msg;

	Register r;
	r.header.msgId = 0x0100;
	r.header.property = 31;
	r.header.msgSerialNumber =333;
	memcpy(r.header.phoneNumber ,s ,6);
	r.plateNumber="abcde";

	int len;
	printf("\nlen:%d   \n",len);
	len = r.toStream(ori);
	printf("\nlen:%d   \n",len);
	int l;
	len = addCheckCode(ori,len);

	toComposedMsg(ori,len, msg.stream, &(msg.len));
	msg.resendTimes=0;
	msg.msgSerialNumber = r.header.msgSerialNumber;
	msg.sendChars = 0;
	msg.complete = false;
	pthread_mutex_lock(&mutex);
	msgList.push_front(msg);
	pthread_mutex_unlock(&mutex);
	len=pr.toStream(ori);
	len = addCheckCode(ori,len);
	toComposedMsg(ori,len,msg.stream, &(msg.len));
	msg.msgSerialNumber = pr.header.msgSerialNumber;
	pthread_mutex_lock(&mutex);
	msgList.push_front(msg);
	pthread_mutex_unlock(&mutex);

}
int TcpClient::open(char * server_ip, char * server_port)
{
		if( (socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
               printf("create socket error: %s(errno:%d)\n)",strerror(errno),errno);  
               return -1;
        }  
  
        memset(&server_addr,0,sizeof(server_addr));  
        server_addr.sin_family = AF_INET;  
        server_addr.sin_port = htons(atoi(server_port));  
  
        if( inet_pton(AF_INET,server_ip,&server_addr.sin_addr) <=0 ) {  
                printf("inet_pton error for %s\n",server_ip);  
                return -1;  
        }  
  
        if( connect(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0) {  
                printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
                return -1;  
        }  
  




       return 0;  
}
int TcpClient::toOriginalMsg(unsigned char * composed,int comlen, unsigned char * original, int *origlen)
{
	int j = 0;
	int start = 0;
	int tmplen = 0;
	
	for(int i=0;i<comlen;i++)
	{
		if ((composed[i] == 0x7e)&&(i != comlen-1))
		{
			start = i + 1;
			continue;
		}
		if ((composed[i] == 0x7e)&&(i == comlen-1))
		{
			if(tmplen != 0)
			{
            			memcpy(&original[j], &composed[start],tmplen);
			}
			j = j + tmplen;
			continue;
		}
		if (composed[i] != 0x7d)
		{
			tmplen++;
		}
		else
		{
			printf("j=%d  tmplen= %d, start=%d\n", j, tmplen, start);
			if(tmplen != 0)
			{
            			memcpy(&original[j], &composed[start],tmplen);
			}
			if(composed[i+1] == 0x02)
			{
				original[j+tmplen] = 0x7e;
			}
			else if(composed[i+1] == 0x01)
			{
				original[j+tmplen] = 0x7d;
			}
			else
			{
				return -1;
			}
			j = j + tmplen + 1;
			start = start + tmplen + 2; 
			i++;
			tmplen = 0;
		}
	}
	*origlen = j;
	
	
	return 0;
}

int TcpClient::toComposedMsg(unsigned char * original,int origlen, unsigned char * composed, int* comlen)
{
	int j = 0;
	int tmplen  = 0;
	int start = 0;

    composed[j] = 0x7e;
	j++;  
	for(int i =0; i< origlen; i++)
	{
		if((original[i] != 0x7e)&&(original[i]!=0x7d))
		{
			tmplen++;		
		}
		else
		{
		//printf("j=%d  tmplen= %d, start=%d\n", j, tmplen, start);		
		    if(tmplen != 0)
		    {
		    	memcpy(composed+j, original+start, tmplen);
		    }
			start =start+tmplen+1;
			if(original[i] == 0x7e)
			{
				composed[j+tmplen] = 0x7d;
				composed[j+tmplen+1] = 0x02;
			}
			if(original[i] == 0x7d)
			{
				composed[j+tmplen] = 0x7d;
				composed[j+tmplen+1] = 0x01;
			}
			
			j=j+tmplen+2;
			tmplen =0;
		}
	}

	if(tmplen != 0)
	{
	   	memcpy(composed+j, original+start, tmplen);
		
	}
	j = j + tmplen;
	composed[j]=0x7e;
	j++;
	
	*comlen = j;
	return 0;
}

int TcpClient::addCheckCode(unsigned char * original, int len)
{
	unsigned char c= 0x00;
	for(int i = 0;i<len; i++)
	{
		c= c ^original[i];
	}
	original[len] = c;
	return len+1;
}

int TcpClient::checkCode(unsigned char * original, int len)
{
	unsigned char c=0x00;
	if(len<2)
	{
		return -2;
	}
	for(int i=0; i<len-1;i++)
	{
		c = c^original[i];
	}
	if(original[len-1] != c)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}


void TcpClient::setTime(BCD (&bcd)[6])
{
	time_t timer;
	struct tm *tblock;
	timer=time(NULL);
	tblock=localtime(&timer);
	bcd[0] = (unsigned int)((tblock->tm_year-100)/10)<<4|(unsigned int)((tblock->tm_year-100)%10);
	bcd[1] = (unsigned int)((tblock->tm_mon+1)/10)<<4 | (unsigned int)((tblock->tm_mon+1)%10);
	bcd[2] = (unsigned int)(tblock->tm_mday/10)<<4 | (unsigned int)((tblock->tm_mday)%10);
	bcd[3] = (unsigned int)(tblock->tm_hour/10)<<4 | (unsigned int)((tblock->tm_hour)%10);
	bcd[4] = (unsigned int)(tblock->tm_min/10)<<4 | (unsigned int)((tblock->tm_min)%10);
	bcd[5] = (unsigned int)(tblock->tm_sec/10)<<4 | (unsigned int)((tblock->tm_sec)%10);

}






