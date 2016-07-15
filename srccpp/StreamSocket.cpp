/*
 * StreamSocket.cpp
 *
 *  Created on: 2016-7-14
 *      Author: xx
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "StreamSocket.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET		-1
#endif

StreamSocket::StreamSocket():mSock(INVALID_SOCKET),mRemoteSock(INVALID_SOCKET),mRemoteIP(" "), mRemotePort(0)
{

}

StreamSocket::StreamSocket(int remoteSock, string remoteIP,  unsigned int remotePort):mSock(INVALID_SOCKET)
{
	mRemoteSock = remoteSock;
	mRemoteIP = remoteIP;
	mRemotePort = remotePort;
}

StreamSocket::~StreamSocket()
{
	if(mSock != INVALID_SOCKET)
	{
		::close(mSock);
		mSock = INVALID_SOCKET;
	}
}

int StreamSocket::startServer(unsigned int port)
{
	int ret;

	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == mSock)
	{
		perror("\n StreamSocket create socket failed :");
	//	throw "socket() failed";
		return -1;
	}

	//bind address
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	ret = bind(mSock,(struct sockaddr *)&addr,sizeof(addr));

	if(ret < 0)
	{
		perror("\n StreamSocket bind failed :");
		::close(mSock);
		mSock = INVALID_SOCKET;
	}

	//listen
	ret = listen(mSock, 5);
	if(ret < 0)
	{
		perror("\n StreamSocket socket listen failed :");
		::close(mSock);
		mSock = INVALID_SOCKET;
	}

	return ret;
}

int StreamSocket::accept(string &ip,  unsigned int &port)
{
	int connectSock;
	unsigned int addrSize;
	struct sockaddr_in connectIPAddr;

	addrSize = sizeof(connectIPAddr);
	memset(&connectIPAddr, 0, addrSize);

	connectSock = ::accept(mSock, (struct sockaddr*)&connectIPAddr, &addrSize);

	if(connectSock != INVALID_SOCKET)
	{
		ip = inet_ntoa(connectIPAddr.sin_addr);
		port = ntohs(connectIPAddr.sin_port);
	}

	return connectSock;
}

int StreamSocket::receive(int socketfd , char *data, unsigned int datasize, unsigned int timesec)
{
	    int retval = 0;
	    fd_set rfds;
	    struct timeval time_out;
	    int len = -1;

	    if( 0 == timesec)
	    {
	    	return recv(socketfd , data, datasize, 0);
	    }

		FD_ZERO(&rfds);
		FD_SET(socketfd,&rfds);
		time_out.tv_sec = timesec;
		time_out.tv_usec = 0;

		retval = select(socketfd+1,&rfds,NULL,NULL,&time_out);
		if(retval > 0)
		{
			if ( FD_ISSET(socketfd, &rfds))
			{
				len = recv(socketfd , data, datasize, 0);
			}
		}
		else if(0 == retval)
		{ /* timeout */
			len = 0;
			cout<<" ##@@@@@@@@@StreamSocket::receive time out@@@@@@@@## " <<endl;
		}
		else
		{
			cout<<"###@@@@@StreamSocket::receive select < 0 @@@@@@###"<<endl;
		}

		return len;
}

int StreamSocket::send(int socketfd, string &data)
{
	return ::send(socketfd, data.c_str(), data.size(), 0);
}

int StreamSocket::close(int socketfd)
{
	return ::close(socketfd);
}

void StreamSocket::setRemoteSocket(int socketfd, string ip,  unsigned int port)
{
	mRemoteSock = socketfd;
	mRemoteIP = ip;
	mRemotePort = port;
}

int StreamSocket::getRemoteSocket()
{
	return mRemoteSock;
}

string &StreamSocket::getRemoteIP()
{
	return mRemoteIP;
}

unsigned int StreamSocket::getRemotePort()
{
	 return mRemotePort;
}
