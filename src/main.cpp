#include <iostream>  
//#include "tcpclient.h"
  
using namespace std;  
#include "Util.h"

#include "Selector.h"

void testUtil() {
	char gbk[] = {(char)0xd6, (char)0xd0, (char)0xce, (char)0xc4};
	GbkString s(gbk,4);
	string s2 = Util::gbkToUtf8(s);
	cout<<s2<<endl;

	s2 = "ÖÐÎÄ";
	s = Util::utf8ToGbk(s2);
	for(int i=0;i<(int)s.length();i++) {
		if(s[i]!=gbk[i]) {
			throw "testUtil failed 1";
		}
	}

	s2 = Util::toHexStr(gbk, 4);
	cout<<s2<<endl;
}

#include "Buffer.h"

void testSendBuffer() {
	Buffer buf(100);
	buf.addByte(1);
	buf.removeByte(1);
	buf.addByte(1);
	buf.removeByte(1);
	Util::assertTrue(buf.isEmpty(), "t1");

	char a[] = {0,1,2,3,4};
	buf.addByte(a,5);
	char b[]={5,6,7,8,9};
	buf.addByte(b,5);

	buf.removeByte(5);
	buf.removeByte(5);

	Util::assertTrue(buf.getRemoveTotal()==12, "remove err");
	Util::assertTrue(buf.getAddTotal()==12, "add err");
	//buf.clear();
	Util::assertTrue(buf.getCount()==0, "empty err");

	buf.clear();

	int len = buf.writeInt(2);
	Util::assertTrue(len==4, "1");
	len = buf.writeShort(3);
	Util::assertTrue(len==2, "2");
	len = buf.writeBcd("12345678", 6);
	Util::assertTrue(len==6, "");

	len = buf.getCount();
	Util::assertTrue(len==12, "");

	int i;
	len = buf.readInt(i);
	Util::assertTrue(len==4 && i==2, "3");

	short s;
	len = buf.readShort(s);
	Util::assertTrue(len==2 && s==3, "3");

	string ss;
	len = buf.readBcd(6, ss);
	Util::assertTrue(len==6 && ss=="000012345678", "3");
}

int main(int argc,char* argv[])  
{
	try {
		testUtil();
		cout<<"testUtil OK"<<endl;
		testSendBuffer();
		cout<<"testSendBuffer OK"<<endl;
		return 0;
	}
	catch(const char* s) {
		cout<<"Error: " << s <<endl;
	}
	return 1;
}  

