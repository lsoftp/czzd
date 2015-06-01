/*
 * SocketListener.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef SOCKETLISTENER_H_
#define SOCKETLISTENER_H_

#include "msgh.h"

class SocketListener {
public:
	SocketListener() {
	}

	virtual ~SocketListener() {
	}

	// this is called after recv a Request* req
	void onRecvRequest(Request* req, Response* resp) {
	}

	// this is called after recv a Response* resp
	void onRecvResponse(Request* req, Response* resp) {
	}

	// this is called after sent a Request* req
	void onSentRequest(Request* req) {
	}

	// this is called after sent a Response* resp
	void onSentResponse(Request* req, Response* resp) {
	}

	// called on io error
	void onError(int error) {
	}

	/**
	 * when select() timeout, this function will be called.
	 * use this function to send heart beat msg, etc.
	 * @param interval - in ms
	 */
	void onTimeout(int interval) {
	}

	/**
	 * this function is called when some bytes received.
	 */
	void onByteRecv(int howmany) {
	}

	/**
	 * this function is called when some bytes sent.
	 */
	void onBytesSent(int howmany) {
	}
};

#endif /* SOCKETLISTENER_H_ */
