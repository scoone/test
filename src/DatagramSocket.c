//============================================================================
// Name        : DatagramSocket.c
// Author      :
// Version     :
// Copyright   : xiewx
// Description :
//============================================================================
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

//#define UDP_DEBUG_ENABLE		(1)


static int mSock = INVALID_SOCKET;
static struct sockaddr_in mOutaddr;

int datagramSocketInit(const char *address, int port)
{
	int ret;
	struct sockaddr_in addr;

	mSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(INVALID_SOCKET == mSock)
	{
		perror("\n datagramSocketInit create socket failed :");
		return -1;
	}

	//bind address
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	//address to use for sending
	memset(&mOutaddr, 0, sizeof(mOutaddr));
	mOutaddr.sin_family = AF_INET;
	mOutaddr.sin_addr.s_addr = inet_addr(address);
	mOutaddr.sin_port = htons(port);

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
		return -1;
	}

	return 0;
}

int datagramSocketTerminate()
{
	if(mSock != INVALID_SOCKET)
	{
		close(mSock);
		mSock = INVALID_SOCKET;
	}

	return 0;
}

int datagramSocketRecevie(stAddress *address, char *buffer, unsigned int buffersize)
{
	struct sockaddr_in sender;
	unsigned int senderSize = sizeof(sender);

	if(NULL == buffer)
	{
		printf("%s : buffer is NULL \n",__FUNCTION__);
		return -1;
	}

#if UDP_DEBUG_ENABLE
	printf("r===============1================%d\n", mSock);
#endif

	int ret = recvfrom(mSock, buffer, buffersize, 0, (struct sockaddr *)&sender, &senderSize);
	if(ret < 0)
	{
		perror("\n datagramSocketRecevie recvfrom failed:");
		return -1;
	}
#if UDP_DEBUG_ENABLE
	printf("r===============2================%d\n", ret);
#endif
	if(address)
	{
		snprintf(address->ip, 16, "%s", inet_ntoa(sender.sin_addr));
		address->port = ntohs(sender.sin_port);
	}
#if UDP_DEBUG_ENABLE
	printf("%s:%d>>%s (ret = %d)\n", inet_ntoa(sender.sin_addr),  ntohs(sender.sin_port), buffer, ret);
#endif
	return ret;
}

int datagramSocketSend(char *buffer, unsigned int buffersize)
{
#if UDP_DEBUG_ENABLE
	printf("s===============1================%d\n", mSock);
#endif
	int ret = sendto(mSock, buffer, buffersize, 0, (struct sockaddr *)&mOutaddr,sizeof(mOutaddr));
	if(ret < 0)
	{
		perror("\n  datagramSocketSend sendto failed : ");
	}
#if UDP_DEBUG_ENABLE
	printf("s===============2================%d, (ret = %d)\n", mSock, ret);
#endif
	return ret;
}

int datagramSocketSendto(const stAddress *address,  char *buffer, unsigned int buffersize)
{
	int ret = -1;
	struct sockaddr_in addr;

	if(NULL == address)
	{
		printf("datagramSocketSendto -- NULL == address \n ");
		return -1;
	}

	if(NULL == buffer)
	{
		printf("datagramSocketSendto -- NULL == buffer \n ");
		return -1;
	}
#if UDP_DEBUG_ENABLE
	printf("sto===============1================%d (%s:%d)\n", mSock, address->ip, address->port);
#endif
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(address->ip);
	addr.sin_port = htons(address->port);

	ret = sendto(mSock, buffer, buffersize, 0, (struct sockaddr *)&addr,sizeof(addr));
	if(ret < 0)
	{
		perror("\n datagramSocketSendto sendtoto failed : ");
	}
#if UDP_DEBUG_ENABLE
	printf("sto===============2================%d, (ret = %d)\n", mSock, ret);
#endif
	return ret;
}

