
#include "tcpclient.h"
#include "datastruct.h"
  
TcpClient::TcpClient()
{  

  
}  

TcpClient::~TcpClient()
{

}

void * TcpClient::threadFunc(void* arg)
{
	TcpClient *ptc = (TcpClient*)arg;
	int sfd=ptc->socket_fd;
	   fd_set readset, writeset;

	   struct timeval timeout={0,200};
	   int maxfd;
	   int ret;
	   while (1)
	   {
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

	         }
	          if (FD_ISSET(sfd, &writeset))
	         {

	         }

         }
     }



	return NULL;
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
  
        int ret = pthread_create(&threadHandler, NULL, threadFunc, this);
        if(ret != 0)
        {
            return ret;
        }
        pthread_join(threadHandler, NULL);
        //printf("send message to server: \n");
     /*   //fgets(message,4096,stdin);
        unsigned char s[6] = {0x01,0x39,0x16,0x85,0x96,0x14};
        Register r;
        r.header.msgId = 0x0100;
        r.header.property = 31;
        memcpy(r.header.phoneNumber ,s ,6);
        r.plateNumber="abcde";
        unsigned char ori[1024],com[1024];
        int len =r.toStream(ori);
        int l;
        len = addCheckCode(ori,len);
        toComposedMSg(ori,len, com, &l);
        if( send( socket_fd,com, l,0 ) < 0 ) {
                printf("send message error\n");  
                return -1;  
        }  
  */
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

int TcpClient::toComposedMSg(unsigned char * original,int origlen, unsigned char * composed, int* comlen)
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

