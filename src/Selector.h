/*
 * Selector.h
 *
 *  Created on: May 31, 2015
 *      Author: chenzero
 */

#ifndef SELECTOR_H_
#define SELECTOR_H_

#include "Handle.h"
#include "Util.h"

class Selector {
protected:
	/**
	 * sockets
	 */
	vector<Handle*> sockets;

public:
	Selector() {

	}

	~Selector() {
		// TODO Auto-generated destructor stub
	}

	void addSocket(Handle* sock) {
		sockets.push_back(sock);
	}
	void removeSocket(Handle* sock) {
		for(size_t i=0;i<sockets.size();i++) {
			if (sockets[i]==sock) {
				sockets.erase(sockets.begin()+i);
			}
		}
	}

	/**
	 * get max socket fd
	 */
	int getMaxFd() {
		int max = -1;
		for(size_t i=0;i<sockets.size();i++) {
			if (sockets[i]->getFd() > max) {
				max = sockets[i]->getFd();
			}
		}
		return max;
	}


	void run() {
		fd_set fds;
		FD_ZERO(&fds);

		while(true) {

		}



	}
};

#endif /* SELECTOR_H_ */
