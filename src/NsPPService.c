//============================================================================
// Name        : NsPPService.c
// Author      :
// Version     :
// Copyright   : xiewx
// Description :nscreen private protocol service
//============================================================================
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
#include <pthread.h>

#include "NsPPService.h"
#include "DatagramSocket.h"
#include "NscreenPrivateProtocolData.h"

typedef void * (threadFunc)(void *arg);

typedef struct _streamSocketInfo
{
    int socketfd;
    unsigned int port;
	char ip[16];
}stStreamSocketInfo;

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

static int nsStreamSocketCommandProcess(const stStreamSocketInfo *ssocket, char *command)
{
	if('\0' == command[0]) //?????????
	{
		return 0xff;
	}

    int ret = 0;
    char response[32] = {0};
    char * s = strsep(&command, STREAM_PACKET_SPLIT);

    switch(atoi(s))
    {
        case EN_CMD_ISONLINE:
            snprintf(response, sizeof(response), "%d>>%s", EN_CMD_ISONLINE, ONLINE);
//            ret = send(ssocket->socketfd, response, sizeof(response), 0);
            break;
        case EN_CMD_MEDIA_PLAY:
        	if(s = strsep(&command, STREAM_PACKET_SPLIT))
        	{
        		printf("\t EN_CMD_MEDIA_PLAY: %s \n", s);
        	}

        	break;
        case EN_CMD_KEY:
        	do
        	{
        		s = strsep(&command, STREAM_PACKET_SPLIT);
        	}while(command);
        	printf("\t EN_CMD_KEY s: %s  \n", s);
        	//deal with key
        	break;
        case EN_CMD_MUSE:
        {
        	int x = 0;
        	int y = 0;
        	do
			{
				s = strsep(&command, STREAM_PACKET_SPLIT);
				if(s[0] != '\0')
				{
					break;
				}
			}while(command);
        	x = atoi(s);

        	do
			{
				s = strsep(&command, STREAM_PACKET_SPLIT);
			}while(command);
        	y = atoi(s);
        	printf("\t EN_CMD_MUSE x,y: (%d,%d) \n", x,y);
        	//deal with mouse
        }
        	break;
        case EN_CMD_INPUTSTR:
        	printf("\t EN_CMD_INPUTSTR : %s \n", s);
        	break;
        default:
        	 printf("address is %s:%d (msg: %s) \n \t s:%s \n", ssocket->ip, ssocket->port, command, s);
            break;
    }

    return ret;
}

static void* recvDataLoop(void *arg)
{
	NPP_TRACE;
    stStreamSocketInfo stSSInfo = *((stStreamSocketInfo *)arg);
	int sock = stSSInfo.socketfd;
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
                    perror("\n  recvDataLoop recv zero len messg: ");
                    break;
                }
                else if ( len == -1 )
                {
                    if ( errno == -EINTR )
                    {
                        continue;
                    }
                    perror("\n recvDataLoop  recv error messg: ");
                    break;
                }

                buff[len] = '\0';
                heart_count = 0;

                if(-1 == nsStreamSocketCommandProcess(&stSSInfo, buff))
                {
                	perror("\n nsStreamSocketCommandProcess send fail!:");
                	//break;
                }

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

    stStreamSocketInfo stSSInfo;

	while(1)
	{
		connectSock = accept(mSock, (struct sockaddr*)&connectIPAddr, &mAddrSize);
		printf("connect ip = %s \n", inet_ntoa(connectIPAddr.sin_addr));
		if(INVALID_SOCKET == connectSock)
		{
			perror("\n connectMainLoop accept socket error: ");
			usleep(300*1000); // 300ms
			continue;
		}

        stSSInfo.socketfd = connectSock;
        stSSInfo.port = ntohs(connectIPAddr.sin_port);
        snprintf(stSSInfo.ip, sizeof(stSSInfo.ip), "%s", inet_ntoa(connectIPAddr.sin_addr));

		createThread(recvDataLoop, (void*)&stSSInfo);
	}

GO_EXIT:
	close(mSock);
	mSock = INVALID_SOCKET;
	return NULL;
}


void nsPPServiceOnLine(void)
{
	stDatagramPPPacket stPacket = {"1", " ", "TV-100", "TV", "1"};
	char onLineMsg[PACKET_SIZE];
	snprintf(onLineMsg, PACKET_SIZE, "%s:%s:%s:%s:%s:%s", stPacket.version, stPacket.packetNo, stPacket.senderName,
			stPacket.senderType, stPacket.commandNo, stPacket.expand);
	onLineMsg[PACKET_SIZE-1] = '\0';

	datagramSocketSend(onLineMsg, PACKET_SIZE);
}

stDatagramPPPacket nsDatagramPacketParse(char *packet, const char *split)
{
	stDatagramPPPacket stPacket;
    char * s = strsep(&packet, split);

    if(s)
    {
    	snprintf(stPacket.version, sizeof(stPacket.version), "%s", s);
    }

    s = strsep(&packet, split);
    if(s)
	{
		snprintf(stPacket.packetNo, sizeof(stPacket.packetNo), "%s", s);
	}

    s = strsep(&packet, split);
	if(s)
	{
		snprintf(stPacket.senderName, sizeof(stPacket.senderName), "%s", s);
	}

	s = strsep(&packet, split);
	if(s)
	{
		snprintf(stPacket.senderType, sizeof(stPacket.senderType), "%s", s);
	}

	s = strsep(&packet, split);
	if(s)
	{
		snprintf(stPacket.commandNo, sizeof(stPacket.commandNo), "%s", s);
	}

	s = strsep(&packet, split);
	if(s)
	{
		snprintf(stPacket.expand, sizeof(stPacket.expand), "%s", s);
	}

	return stPacket;
}
/*
static char *getTime(char *timeBuf)
{
    time_t timep;
    struct tm *p =NULL;
    time(&timep);
    p=gmtime(&timep);
        if (NULL == p) {
         return NULL;
     }
     snprintf(timeBuf,BUFFERSIZE, "%02d:%02d:%02d:%02d:%02d:%02d",
            p->tm_year % 100, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    return timeBuf;

}
*/
void nsDatagramCommandProcess(const stAddress *revcAddress , const char *command) /* command from pppacket commandNo */
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
	datagramSocketInit(BROADCAST_ADDR, BROADCAST_PORT);

	stDatagramPPPacket stPacketRecived;
	char msg[RECV_BUFFSIZE];
	stAddress recvFrom;

	while(1)
	{
		if(datagramSocketRecevie(&recvFrom, msg, RECV_BUFFSIZE) > 0)
		{
			//printf("msg : %s\n", msg);
			stPacketRecived = nsDatagramPacketParse(msg, DATAGRAM_PACKET_SPLIT);
			nsDatagramCommandProcess(&recvFrom, stPacketRecived.commandNo);
		}

//		usleep(1000*1000); //1s   Don't sleep because datagram recevie is blocked
	}
	return NULL;
}

void nsPPServiceInit(void)
{
	serviceMainThread = createThread(serviceMainLoop, NULL);
	connectMainThread = createThread(connectMainLoop, NULL);
}
