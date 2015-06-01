/*
 * SendBuffer.h
 *
 *  Created on: Jun 1, 2015
 *      Author: chenzero
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "Util.h"

class Buffer {
protected:
	/**
     * the internal buffer
     */
    char* data;

    /**
     * capacity
     */
    int capacity;

    /**
     * the actual data count
     */
    int count;

    /**
     * the data begin offset
     */
    int offset;

    /**
     * add total
     */
    int addTotal;
    /**
     * remove total count
     */
    int removeTotal;

public:

    Buffer(int c) {
		data = new char[c];
		capacity = c;
		offset = 0;
		count = 0;
		addTotal = 0;
		removeTotal = 0;
	}

	~Buffer() {
		delete data;
	}

	void canHold(int len) {
		if(capacity-count< len) {
			throw "can not hold";
		}

		if ((offset+len + count) > capacity) {
			march();
		}
	}

	/**
	 * add a byte in this buffer
	 * @param b
	 */
	void addByte (char b) {
		canHold(1);
		data[offset+count] = b;
		count++;

		addTotal+=1;
	}

	/**
	 * add a byte array in this buffer
	 * @param b
	 */
	void addByte (char* b, int len) {
		canHold(len);
		memcpy(data+offset+count, b, len);
		count += len;

		addTotal+=len;
	}

	int getAddTotal() {
		return this->addTotal;
	}

	/**
	 * get empty byte size
	 */
	int getEmptySize () {
		return (capacity - count);
	}

	/**
	 * remove c bytes from this buffer
	 * @param c
	 * @throws IOException
	 */
	void removeByte (int c) {
		if (c > count) {
			throw ("no enough bytes");
		}

		offset = offset + c;
		count = count - c;

		this->removeTotal += c;
	}

	int getRemoveTotal() {
		return this->removeTotal;
	}

	/**
	 * get "empty" byte size after data
	 * @return
	 */
	int getEmptySizeAfterData () {
		return (capacity - (offset+count));
	}

	/**
	 * get the whole buffer size
	 * @return
	 */
	int getCapacity () {
		return capacity;
	}

	/**
	 * get internal data
	 * @return
	 */
	char* getData () {
		return data;
	}

	int getOffset() {
		return offset;
	}


	/**
	 * get the empty start index. (data can put from this start index)
	 * @return
	 */
	int getEmptyStartIndex() {
		return offset+count;
	}

	/**
	 * return how many bytes in this buffer
	 * @return
	 */
	int getCount () {
		return count;
	}

	/**
	 * check if any bytes in the buffer.
	 * @return
	 */
	bool isEmpty() {
		return count == 0;
	}

	/**
	 * clear all bytes
	 */
	void clear () {
		offset = 0;
		count = 0;
		this->removeTotal += count;
	}

	/**
	 * move the data to the beginning.
	 */
	void march() {
		if (offset==0) {
			return;
		}
		if (count==0) {
			offset=0;
			return;
		}
		memmove(data, data+offset, count);
		offset = 0;
	}

	//------------------------------------------------------------------------
	// networking functions
	//------------------------------------------------------------------------

	/**
	 * write an integer
	 */
	int writeInt(int i) {
		canHold(4);
		i = htonl(i);
		addByte((char*)&i, 4);
		return 4;
	}

	/**
	 * read an int
	 */
	int readInt(int& i) {
		if(count<4) {
			throw "data more";
		}
		int* p = (int*)(data+offset);
		i = ntohl(*p);
		removeByte(4);
		return 4;
	}

	/**
	 * write a short int
	 */
	int writeShort(short i) {
		canHold(2);
		i = htons(i);
		addByte((char*)&i, 2);
		return 2;
	}
	/**
	 * read a short
	 */
	int readShort(short& i) {
		if(count<2) {
			throw "data more";
		}
		short* p = (short*)(data+offset);
		i = ntohs(*p);
		removeByte(2);
		return 2;
	}

	/**
	 * write a num as bcd code
	 * @param len - the bcd len
	 */
	int writeBcd(string s, int len) {
		canHold(len);

		int n = s.length()/2;

		// append 0 if need
		if(n<len) {
			for(int i=0;i<len-n;i++) {
				addByte(0);
			}
		}
		else {
			n = len;
		}

		char c;
		for(int i=0;i<n; i++) {
			c = (s[i*2]-'0')<<4 | (s[i*2+1]-'0');
			addByte(c);
		}
		return len;
	}

	/**
	 * read a bcd code.
	 * @param len: the expected bcd len
	 */
	int readBcd(int len, string& s) {
		int h,l;
		std::ostringstream out;

		if(count<len) {
			throw "more data";
		}
		for(int i=0;i<len;i++) {
			h = (data+offset)[i]>>4;
			l = (data+offset)[i]&0xf;

			out<<Util::HEX_DIGITS[h]<<Util::HEX_DIGITS[l];
		}
		s = out.str();
		return len;
	}


};


#endif /* BUFFER_H_ */
