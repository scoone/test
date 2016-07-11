//============================================================================
// Name        : NsPPService.c
// Author      :
// Version     :
// Copyright   : xiewx
// Description :nscreen private protocol service
//============================================================================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "NsPPService.h"
#include "DatagramSocket.h"
#include "NscreenPrivateProtocolData.h"

typedef void * (threadFunc)(void *arg);

static pthread_t serviceMainThread;
static pthread_t connectMainThread;

static pthread_t createThread(threadFunc looper, void *arg)
{
	const int pthreadStackSizeDefault = 64*1024;  // 64K

	int ret = -1;
	pthread_t id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, pthreadStackSizeDefault);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&id, &attr, looper, arg);
	pthread_attr_destroy(&attr);

	if(ret < 0)
	{
		perror("\n createThread failed");
	}

	return id;
}



static void* recvDataLoop(void *arg)
{
	NPP_TRACE;
	int sock = *((int *)arg);
    int retval = 0;
    int heart_count = 0;
    fd_set rfds;
    struct timeval time_out;
    char buff[RECV_BUFFSIZE];
    int len;

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(sock,&rfds);
        time_out.tv_sec = 45;
        time_out.tv_usec = 0;

        retval = select(sock+1,&rfds,NULL,NULL,&time_out);
        if(retval > 0) {
            if ( FD_ISSET(sock, &rfds))
            {
                len = recv(sock , buff, sizeof(buff), 0);
                if( 0 == len)
                {
                    perror("\n recv zero len messg: ");
                    break;
                }
                else if ( len == -1 )
                {
                    if ( errno == -EINTR )
                    {
                        continue;
                    }
                    perror("\n  recv error messg: ");
                    break;
                }

                buff[len] = '\0';
                heart_count = 0;

                if(-1 == send(sock, "ok", sizeof("ok"), 0))
                { // same as write(sock ,/*rev_rsp*/"ok",sizeof("ok")) in linux
                	perror("\n  send fail!:");
                    break;
                }

                printf("recv------->%s \n", buff);

                memset(buff,0,sizeof(buff));
            }
        }
        else if(0 == retval)
        { /* timeout */
            heart_count++;
            if ( heart_count < 2 )
            {
               // sendHeartBeatPacket( sock );
                continue;
            }
            printf("\n###########@@@@@@@@@ time out@@@@@@@@############### heart_count = %d \n", heart_count);
          //  break;
        }
        else
        {
            if ( errno == -EINTR) {
                continue;
            }
            else {
            	printf("\n###########@@@@@@@@@ select < 0 @@@@@@@@############### \n");
            	break;
            }
        }
    }
    printf("\n###########@@@@@@@@@ exit @@@@@@@@############### \n");
	close(sock);
    return NULL;
}

void *connectMainLoop(void *arg)
{
	NPP_TRACE;
	int ret;
	int mSock;
	int connectSock;

	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == mSock)
	{
		perror("\n connectMainLoop create socket failed :");
		return NULL;
	}

	//bind address
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(CONNECT_PORT);

	ret = bind(mSock,(struct sockaddr *)&addr,sizeof(addr));

	if(ret < 0)
	{
		perror("\n connectMainLoop bind failed :");
		goto GO_EXIT;
	}

	//listen
	ret = listen(mSock, 5);
	if(ret < 0)
	{
		perror("\n connectMainLoop socket listen failed :");
		goto GO_EXIT;
	}

	unsigned int mAddrSize;
	struct sockaddr_in connectIPAddr;

	mAddrSize = sizeof(connectIPAddr);
	memset(&connectIPAddr, 0, mAddrSize);

	while(1)
	{
		NPP_TRACE;
		connectSock = accept(mSock, (struct sockaddr*)&connectIPAddr, &mAddrSize);
		printf("connect ip = %s \n", inet_ntoa(connectIPAddr.sin_addr));
		if(INVALID_SOCKET == connectSock)
		{
			perror("\n connectMainLoop accept socket error: ");
			NPP_TRACE;
			usleep(300*1000); // 300ms
			continue;
		}

		createThread(recvDataLoop, (void*)&connectSock);
	}

GO_EXIT:
	close(mSock);
	mSock = INVALID_SOCKET;
	return NULL;
}


void nsPPServiceOnLine(void)
{
	stPPPacket stPacket = {"1", " ", "TV-100", "TV", "1"};
	char onLineMsg[PACKET_SIZE];
	snprintf(onLineMsg, PACKET_SIZE, "%s:%s:%s:%s:%s:%s", stPacket.version, stPacket.packetNo, stPacket.senderName,
			stPacket.senderType, stPacket.commandNo, stPacket.expand);
	onLineMsg[PACKET_SIZE-1] = '\0';

	datagramSocketSend(onLineMsg, PACKET_SIZE);
}

stPPPacket nsPacketParse(char *packet, const char *split)
{
	stPPPacket stPacket;
    char * s = strsep(&packet, PACKET_SPLIT);

    if(s)
    {
    	snprintf(stPacket.version, sizeof(stPacket.version), "%s", s);
    }

    s = strsep(&packet, PACKET_SPLIT);
    if(s)
	{
		snprintf(stPacket.packetNo, sizeof(stPacket.packetNo), "%s", s);
	}

    s = strsep(&packet, PACKET_SPLIT);
	if(s)
	{
		snprintf(stPacket.senderName, sizeof(stPacket.senderName), "%s", s);
	}

	s = strsep(&packet, PACKET_SPLIT);
	if(s)
	{
		snprintf(stPacket.senderType ,  sizeof(stPacket.senderType), "%s", s);
	}

	s = strsep(&packet, PACKET_SPLIT);
	if(s)
	{
		snprintf(stPacket.commandNo ,  sizeof(stPacket.commandNo), "%s", s);
	}

	s = strsep(&packet, PACKET_SPLIT);
	if(s)
	{
		snprintf(stPacket.expand ,  sizeof(stPacket.expand), "%s", s);
	}

	return stPacket;
}

void nsCommandProcess(const stAddress *revcAddress , const char *command) /* command from pppacket commandNo */
{
	//printf("command : %s\n", command);
	switch(atoi(command))
	{
		case EN_CMD_ONLINE:
		{
			char response[]="1:1468035052270:TV-100:TV:3:TV-100";
			datagramSocketSendto(revcAddress, response, sizeof(response));
		}
		break;

		default:
			break;
	}
}

void *serviceMainLoop(void *arg)
{
	NPP_TRACE;

	datagramSocketInit(BROADCAST_ADDR, BROADCAST_PORT);

	stPPPacket stPacketRecived;
	char msg[RECV_BUFFSIZE];
	stAddress recvFrom;

	while(1)
	{
		if(datagramSocketRecevie(&recvFrom, msg, RECV_BUFFSIZE) > 0)
		{
			//printf("msg : %s\n", msg);
			stPacketRecived = nsPacketParse(msg, PACKET_SPLIT);
			nsCommandProcess(&recvFrom, stPacketRecived.commandNo);
		}

//		usleep(1000*1000); //1s   Don't sleep because datagram recevie is blocked
	}
	return NULL;
}

void nsPPServiceInit(void)
{
	NPP_TRACE;
	serviceMainThread = createThread(serviceMainLoop, NULL);
	connectMainThread = createThread(connectMainLoop, NULL);
}
