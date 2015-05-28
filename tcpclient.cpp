
#include "tcpclient.h"
#include "datastruct.h"

list<Msg> TcpClient::msgList;
pthread_mutex_t TcpClient::mutex;
list<RecvStream> TcpClient::recvStreamList;
pthread_mutex_t TcpClient::mutexRecvStream;
list<WORD> TcpClient::serialNumberList;
pthread_mutex_t TcpClient::mutexserialNumber;
WORD TcpClient::m_serialNumber = 0;
BYTE TcpClient::m_phoneNumber[6] = {0x01,0x39,0x16,0x85,0x96,0x14};
int TcpClient::m_timeout = 2500000;
int TcpClient::m_resendtimes = 3;
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

}

void * TcpClient::sendRecv(void* arg)
{
		TcpClient *ptc = (TcpClient*)arg;
		int sfd=ptc->socket_fd;
	   fd_set readset, writeset;
	   Msg *pmsg=NULL;
	   RecvBuf recvbuf;
	   RecvStream recvstream;
	   struct timeval timeout={0,200},endtime;
	   int maxfd;
	   int ret, sendChars,recvChars, r;

	   while (1)
	   {

		 handleMsgList();


		 if(pmsg = NULL)
		 {
			 pmsg = getMsgToSend();
		 }
		 else
		 {
			 if(pmsg->complete)
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
	        		 r = recvbuf.getDataFromBuf(recvstream.stream, &(recvstream.size));
	        		 if(r)
	        		 {
	        			 //printf("recvchars %d\n",recvChars);
	        			 pthread_mutex_lock(&mutexRecvStream);
	        			 recvStreamList.push_front(recvstream);
	        			 pthread_mutex_unlock(&mutexRecvStream);
	        		 }
	        	 }
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
	        	  }
	         }

         }
     }


	//pthread_detach(pthread_self());
	return NULL;
}

void * TcpClient::handleRecvMsg(void *arg)
{
	TcpClient *ptc = (TcpClient*)arg;
	RecvStream *prs=NULL;
	RecvStream rs;
	int i;
	//ptc->checkCode(rs.stream, rs.size);
	while(1)
	{
		pthread_mutex_lock(&mutexRecvStream);
		if(recvStreamList.size()>0)
		{
			prs =&(*recvStreamList.begin());
			recvStreamList.pop_front();
			printf("prs len:%d\n", prs->size);
			fflush(stdout);
		}
		pthread_mutex_unlock(&mutexRecvStream);
		if (prs != NULL)
		{
			ptc->toOriginalMsg(prs->stream, prs->size, rs.stream, &rs.size);
			i = ptc->checkCode(rs.stream, rs.size);
		}

//
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
			default:break;
			}
		}

	}

}

void TcpClient::handleRegisterAck(RecvStream *prs)
{
	RegisterAck ra;
	int j = ra.fromStream(prs->stream, prs->size);
	WORD serialNumber = ra.header.msgSerialNumber;
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
	Msg *pmsg=NULL;
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
			break;
		}
		else if(time >= m_timeout)
		{
			if(pmsg->resendTimes <m_resendtimes)
			{
				pmsg->sendChars = 0;
				pmsg->complete = false;
				break;
			}

		}
	}
	pthread_mutex_unlock(&mutex);
	return pmsg;
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


			if((pmsg->msgSerialNumber = sn) ||(pmsg->resendTimes == m_resendtimes-1))
			{
				msgList.erase(it1++);
				if(pmsg->resendTimes == m_resendtimes-1)
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

        printf("\nret:%d   threadid: %d\n",ret,sendRecvHandler);
        //pthread_join(sendRecvHandler, NULL);
        //printf("send message to server: \n");
        //fgets(message,4096,stdin);
        Msg msg;
        unsigned char s[6] = {0x01,0x39,0x16,0x85,0x96,0x14};
        Register r;
        r.header.msgId = 0x0100;
        r.header.property = 31;
        memcpy(r.header.phoneNumber ,s ,6);
        r.plateNumber="abcde";
        unsigned char ori[1024];
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
        pthread_join(sendRecvHandler, NULL);
        pthread_join(recvHandler, NULL);
       close(socket_fd);  
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









