/*
 * MsgHeader1.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <string>
#include "Buffer.h"
#include <arpa/inet.h>

#include "Util.h"
#include "Message.h"
using namespace std;

class Header : public Message {
public:
	short type;
	short property;
	string mobile;
	short id; // sequence id

	// frag total
	short total;
	// frag no
	short fragNo;


public:
	Header() {
		type=0;
		property=0;
		mobile="";
		id=0;

		total=0;
		fragNo=0;
	}

	~Header() {
		// TODO Auto-generated destructor stub
	}

	bool isFragmented() {
		short t = property & 0x2000; // the 13th bit
		if(t==0) {
			return false;
		}
		return true;
	}

	int toStream(Buffer& buf) {
		buf.canHold(20);

		int len =0;

		len += buf.writeShort(type);

		len += buf.writeShort(property);

		len += buf.writeBcd(mobile, 6);

		len += buf.writeShort(id);

		if (isFragmented()) {

			len += buf.writeShort(total);

			len += buf.writeShort(fragNo);
		}
		return len;
	}

	int fromStream(Buffer& buf)
	{
		if(buf.getCount()<12) {
			throw "more data need";
		}

		int len =0;

		len += buf.readShort(type);

		len += buf.readShort(property);

		len += buf.readBcd(6, mobile);

		len+= buf.readShort(id);

		if (isFragmented()) {
			if(buf.getCount()<16) {
				throw "more data need";
			}
			buf.readShort(total);
			buf.readShort(fragNo);
		}

		return len;
	}

};

#endif /* HEADER_H_ */
