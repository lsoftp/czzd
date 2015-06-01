/*
 * ChunkData.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef MSGDATA_H_
#define MSGDATA_H_

#include "Buffer.h"
#include <string>
using namespace std;

class Message {
public:

	Message() {
	}

	~Message() {
	}

	int fromStream(Buffer& buffer) {
		return 0;
	}

	int toStream(Buffer& buffer) {
		return 0;
	}

	string toString() {
		return "msgdata";
	}
};

#endif /* MSGDATA_H_ */
