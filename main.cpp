#include <iostream>  
#include "tcpclient.h"
#include "config.h"
using namespace std;  
  
int main(int argc,char* argv[])  
{  
		//unsigned char com[11] ={0x7e, 0x30, 0x7d,0x02,0x7d,0x02,0x08,0x7d,0x01,0x55,0x7e};
		//int len =0;
		//unsigned char original[11];
        TcpClient tc;  
       // tc.open("192.168.10.83","8899");
        Config cg;
        //cout<<"id "<<cg.m_paramlist["heartBeat"].id<<"value "<<cg.m_paramlist["heartBeat"].value<<endl;
        tc.start();
		/*tc.toOriginalMsg(com, 11,  original,&len );
		for(int i = 0; i<len; i++)
            printf("%02x ",original[i]);
        printf("\n%d\n",len);

		unsigned char ori[6] = {0x30,0x7e,0x08,0x7d,0x55,0x7e};
		unsigned char compo[15];
		tc.toComposedMsg(ori, 6, compo, &len);
		for(int i = 0; i<len; i++)
            printf("%02x ",compo[i]);
        printf("\n%d\n",len);*/
        for(;;);
        return 0;  
}  

