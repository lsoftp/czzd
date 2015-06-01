/*
 * Util.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <iconv.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <arpa/inet.h>
using namespace std;

/**
 * string in GBK encoding, to print, convert encoding to utf8 by gbkToUtf8()
 */
typedef string GbkString;

class Util {
public:
	Util() {
	}

	virtual ~Util() {
	}

	/**
	 * assert true
	 */
	static void assertTrue(bool t, string msg) {
		if(!t) {
			throw msg.c_str();
		}
	}

	/**
	 * this is not thread-safe
	 */
	static GbkString utf8ToGbk(string& s) {
		static char buf[10001];
		size_t dstlen = 10000;

		iconv_t conv = iconv_open("GB2312", "UTF8");
		if(conv==(iconv_t)-1) {
			throw "utf8ToGBK -1";
		}
		size_t srcLen = s.length();

		/*
		from man: The inbuf argument is the address of a variable that points to the first character of the input sequence;
		in anycases, declear a var to save ws.
		*/
		char* src0 = (char*)s.c_str();
		char* outbuf = buf;
		// Note that it needs to set the buf[dstlen] to 0, but here, just set all
		memset(buf,0,sizeof(buf));
		//int ret =
		iconv(conv, (char**)&src0, &srcLen, &outbuf , &dstlen);
		//printf("iconv ret:%d \n", ret);
		iconv_close(conv);
		return buf;
	}


	static string gbkToUtf8(GbkString& src) {
		static char buf[10000];
		size_t dstlen = 10000;

		iconv_t conv = iconv_open("UTF8", "GB2312");
		if(conv==(iconv_t)-1) {
			throw "utf8ToGBK -1";
		}
		size_t srcLen = (int)src.length();

		/*
		from man: The inbuf argument is the address of a variable that points to the first character of the input sequence;
		in anycases, declear a var to save ws.
		*/
		char* src0 = (char*)src.c_str();
		char* outbuf = buf;
		// Note that it needs to set the buf[dstlen] to 0, but here, just set all
		memset(buf,0,sizeof(buf));
		//int ret =
		iconv(conv, (char**)&src0, &srcLen, &outbuf , &dstlen);
		//printf("iconv ret:%d \n", ret);
		iconv_close(conv);
		return buf;
	}

	static const string HEX_DIGITS[16];

	/**
	 * convert bytes array to hex string
	 */
	static string toHexStr(char* bytes, int len) {
		int h,l;
		std::ostringstream out;

		for(int i=0;i<len;i++) {

			h = (bytes[i]>>4)&0xf;
			l = bytes[i]&0xf;

			out<<HEX_DIGITS[h]<<HEX_DIGITS[l]<< " ";
		}

		return out.str();
	}


};


#endif /* UTIL_H_ */
