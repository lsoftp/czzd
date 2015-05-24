#include <iostream>  
#include <stdio.h>
#include <stdlib.h>
#include "tcpclient.h"
  
using namespace std;  


  
int main(int argc,char* argv[])  
{  
		unsigned char com[9] ={0x7e, 0x30, 0x7d,0x02,0x08,0x7d,0x01,0x55,0x7e};
		int len =0;
		unsigned char original[11];
        tcp_client tc;  
		tc.toOriginalMsg(com, 9,  original,&len );
		for(int i = 0; i<len; i++) {
			printf("%x ",original[i]);
		}
        printf("\n");
        //this is a test
        printf("%d\n",len);
        printf("\n");

        return 0;  
}  

