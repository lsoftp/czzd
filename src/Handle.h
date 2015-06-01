/*
 * Socket.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef HANDLE_H_
#define HANDLE_H_

#include "Util.h"
#include "Message.h"
#include "msgh.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "SocketListener.h"


class Handle {

protected:
	int fd; // the socket fd
	bool listenerSocket; // listener socket flag

	//enum State {SOCKET_CONN, SOCKET_DISCONN};
	//State state;

	string serverIp;
	int port;
	SocketListener* listener;

	bool readable;
	bool writable;

public:
	Handle() {
		fd = -1;
		listenerSocket=false;

		serverIp = "";
		port = 0;
		listener = NULL;

		readable = false;
		writable = false;
	}

	~Handle() {
		close();
	}


	void open() {
		int ret;

		if(fd == -1) {
			fd = socket(AF_INET,SOCK_STREAM,0);
			if(fd == -1) {
				throw "create socket err";
			}
		}

		struct sockaddr_in	servaddr;
		bzero(&servaddr, sizeof(servaddr));

		if(this->listenerSocket) {
			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family      = AF_INET;
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			servaddr.sin_port        = htons(port);
			ret = bind (fd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			if(ret==-1) {
				throw "bind error";
			}
			ret = listen(fd, 5);
			if(ret==-1) {
				throw "listen error";
			}

		}
		else {
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(port);
			inet_pton(AF_INET, serverIp.c_str(), &servaddr.sin_addr);

			ret = ::connect(fd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			if(ret==-1) {
				throw "connect error";
			}
		}
	}

	void close() {
		if(fd!=-1) {
			::close(fd);
			fd = -1;
		}
	}

	int getFd() {
		return fd;
	}

	/**
	 * is this handle readable
	 */
	bool isReadable() {
		return readable;
	}

	/**
	 * is this handle writable
	 */
	bool isWritable() {
		return writable;
	}

	int sendRequest(Request* req) {
		return 0;
	}

	void setListener(SocketListener* sl) {
		this->listener = sl;
	}
	SocketListener* getListener() {
		return listener;
	}

};

#endif /* HANDLE_H_ */
