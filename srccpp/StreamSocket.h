/*
 * StreamSocket.h
 *
 *  Created on: 2016-7-14
 *      Author: xx
 */

#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include <iostream>
#include <string>
using namespace std;

class StreamSocket
{
private:
	int mSock;
	int mRemoteSock;
	string mRemoteIP;
	unsigned int mRemotePort;
public:
	StreamSocket();
	StreamSocket(int remoteSock, string remoteIP,  unsigned int remotePort); /*mRemoteSock =  remoteSock */
	virtual ~StreamSocket();

	int startServer(unsigned int port);
	int accept(string &ip,  unsigned int &port); /* return socketfd */
	int receive(int socketfd , char *data, unsigned int datasize, unsigned int timesec);
	int send(int socketfd, string &data);
	int close(int socketfd);

	void setRemoteSocket(int socketfd, string ip,  unsigned int port);
	int getRemoteSocket();
	string & getRemoteIP();
	unsigned int getRemotePort();
};

#endif /* STREAMSOCKET_H_ */
