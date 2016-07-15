/*
 * DatagramSocket.cpp
 *
 *  Created on: 2016-7-14
 *      Author: xiewx
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "DatagramSocket.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET		-1
#endif

DatagramSocket::DatagramSocket(string ip, unsigned int port)
{
		int ret;
		struct sockaddr_in addr;

		mSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(INVALID_SOCKET == mSock)
		{
			perror("\n datagramSocketInit create socket failed :");
			return;
		}

		//bind address
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		//address to use for sending
		mIP = ip;
		mPort = port;

		int opt = 1;
		int optLen = sizeof(int);

		ret = setsockopt(mSock, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, optLen);//一般在发送UDP数据报的时候，希望该socket发送的数据具有广播特性

		ret = setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, optLen);

		ret = bind(mSock,(struct sockaddr *)&addr,sizeof(addr));

		if(ret < 0)
		{
			close(mSock);
			mSock = INVALID_SOCKET;
			perror("\n datagramSocketInit bind failed :");
		}
}

DatagramSocket::~DatagramSocket()
{
	if(mSock != INVALID_SOCKET)
	{
		close(mSock);
		mSock = INVALID_SOCKET;
	}
}

int DatagramSocket::recevie(string &ip,  unsigned int &port, char *data, unsigned int datasize)
{
	struct sockaddr_in sender;
	unsigned int senderSize = sizeof(sender);

	if(NULL == data)
	{
		cout<<" DatagramSocket::recevie "<<endl;
		throw(" DatagramSocket::recevie NULL == data");
		return -1;
	}

	int ret = recvfrom(mSock, data, datasize, 0, (struct sockaddr *)&sender, &senderSize);
	if(ret < 0)
	{
		perror("\n DatagramSocket::recevie recvfrom failed:");
		return -1;
	}

	ip = inet_ntoa(sender.sin_addr);
	port = ntohs(sender.sin_port);

	return ret;
}

int DatagramSocket::send(string &data)
{
	int ret = -1;
	struct sockaddr_in mOutaddr;

	memset(&mOutaddr, 0, sizeof(mOutaddr));
	mOutaddr.sin_family = AF_INET;
	mOutaddr.sin_addr.s_addr = inet_addr(mIP.c_str());
	mOutaddr.sin_port = htons(mPort);

	ret = ::sendto(mSock, data.c_str(), data.size(), 0, (struct sockaddr *)&mOutaddr,sizeof(mOutaddr));
	if(ret < 0)
	{
		perror("\n  datagramSocketSend sendto failed : ");
	}

	return ret;
}

int DatagramSocket::sendto(string &ip,  unsigned int &port, string &data)
{
	int ret = -1;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	ret = ::sendto(mSock, data.c_str(), data.size(), 0, (struct sockaddr *)&addr,sizeof(addr));
	if(ret < 0)
	{
		perror("\n datagramSocketSendto sendtoto failed : ");
	}

	return ret;
}

