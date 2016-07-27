
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

#define SOCK_LISTEN 5

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

int nsStreamSocketServerStart(int port)
{
    int ret;
    int sock;

    printf("nsStreamSocketServerStart port:%d\n", port);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == sock)
    {        
        perror("\n nsStreamSocketServerStart create socket failed :");
        return INVALID_SOCKET;
    }

    //bind address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    ret = bind(sock,(struct sockaddr *)&addr,sizeof(addr));

    if(ret < 0)
    {
        perror("\n nsStreamSocketServerStart bind failed :");
        close(sock);
        return INVALID_SOCKET;
    }

    //listen
    ret = listen(sock, SOCK_LISTEN);
    if(ret < 0)
    {
        perror("\n nsStreamSocketServerStart socket listen failed :");
        close(sock);
        return INVALID_SOCKET;
    }

    return sock;
}

int nsStreamSocketResponse(int socketfd, char *buffer, unsigned int buffersize)
{
    printf("nsStreamSocketResponse: %s \n", buffer);

   /*first send length*/
    char responseLen[4] = {0};
    responseLen[0] = (buffersize >> 24) & 0xff;
    responseLen[1] = (buffersize >> 16) & 0xff;
    responseLen[2] = (buffersize >> 8) & 0xff;
    responseLen[3] = buffersize & 0xff;
    send(socketfd, responseLen, sizeof(responseLen), 0);
    
    return send(socketfd, buffer, buffersize, 0);
}

static int nsStreamSocketCommandProcess(const stStreamSocketInfo *ssocket, char *command)
{
    printf("address is %s:%d (msg: %s) \n", ssocket->ip, ssocket->port, command);
    int ret = 0;
    char response[32] = {0};
    char * s = strsep(&command, STREAM_PACKET_SPLIT);
    printf("\t s: %s \n", s);
    switch(atoi(s))
    {
        case EN_CMD_ISONLINE:
            snprintf(response, sizeof(response), "%d>>%s", EN_CMD_ISONLINE, ONLINE);
            ret = nsStreamSocketResponse(ssocket->socketfd, response, strlen(response));
            break;
        default:
            break;
    }
    printf("\t\t msg: %s (ret = %d)\n", command, ret);
    return ret;
}

static int nsStreamSocketRecvData(int sock, void *buffer, unsigned int buffersize)
{
        int recvLeft = buffersize;
        int recvLen = 0;
        char *ptr = (char *)buffer;
    
        while (recvLeft > 0)
        {
                recvLen = recv(sock, buffer, recvLeft, 0);
                if (recvLen < 0)
                {
                    perror("\n recvData :");
                        return -1;
                }
                else if (0 == recvLen)
                {
                        printf("recvLeft = %d,recvLen = %d\n", recvLeft, recvLen);
                        return (buffersize-recvLeft);
                }

                recvLeft -= recvLen;
                ptr += recvLen;
                printf("recvLeft = %d,recvLen = %d\n", recvLeft, recvLen);
        }
    
        return buffersize;
}

static void* recvDataLoop(void *arg)
{
    stStreamSocketInfo stSSInfo = *((stStreamSocketInfo *)arg);
        int sock = stSSInfo.socketfd;
    
    char buffLen[4];     // int
    char buff[RECV_BUFFSIZE];
    int len;
/*        
    struct timeval time_out;
    time_out.tv_sec = 15;
    time_out.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&time_out, sizeof(time_out));
*/        
    while(1)
    {
        if(nsStreamSocketRecvData(sock, buffLen, sizeof(buffLen)) != sizeof(buffLen)) // recv length
        {
            printf("recvDataLoop receive data length failed.\n");
            continue;
        }

        len = ntohl(*(unsigned int *)buffLen);
        printf("recvDataLoop receive data length len=%d\n", len);

        if(len > RECV_BUFFSIZE)
        {
            printf("recvDataLoop receive data length > RECV_BUFFSIZE(1024).\n");
        }
        else
        {
            memset(buff,0,sizeof(buff));
            if(nsStreamSocketRecvData(sock, buff, len) != len)
            {
                printf("recvDataLoop receive data *** failed.\n");        
            }
            else
            {
                nsStreamSocketCommandProcess(&stSSInfo, buff);
            } 
        }     
    }
    
    printf("\n###########@@@@@@@@@ exit @@@@@@@@############### \n");
        close(sock);
    return NULL;
}

void *connectMainLoop(void *arg)
{
    int mSock = nsStreamSocketServerStart(CONNECT_PORT);

    if(INVALID_SOCKET == mSock)
    {
        return NULL;
    }

    int connectSock;
        unsigned int mAddrSize;
        struct sockaddr_in connectIPAddr;
    stStreamSocketInfo stSSInfo;

        mAddrSize = sizeof(connectIPAddr);
        memset(&connectIPAddr, 0, mAddrSize);

        while(1)
        {
                connectSock = accept(mSock, (struct sockaddr*)&connectIPAddr, &mAddrSize);
                printf("connect ip = %s \n", inet_ntoa(connectIPAddr.sin_addr));
                if(INVALID_SOCKET == connectSock)
                {
                        perror("\n connectMainLoop accept socket error: ");
                        usleep(300*1000); // 300ms
                }
        else
        {
            stSSInfo.socketfd = connectSock;
            stSSInfo.port = ntohs(connectIPAddr.sin_port);
            snprintf(stSSInfo.ip, sizeof(stSSInfo.ip), "%s", inet_ntoa(connectIPAddr.sin_addr));
            
                createThread(recvDataLoop, (void*)&stSSInfo);
        }
        }

        close(mSock);
        mSock = INVALID_SOCKET;
    
        return NULL;
}


long long getMSTime() // return unit : ms
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec*1000+ ts.tv_nsec / 1000000;
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

void nsDatagramCommandProcess(const stAddress *revcAddress , const char *command) /* command from pppacket commandNo */
{
        //printf("command : %s\n", command);
        switch(atoi(command))
        {
                case EN_CMD_ONLINE:
                {
                        char response[64]= {0}; //"1:1468035052270:TV-100:TV:3:TV-100";
                        snprintf(response, 64, "%s:%lld:TV-110:TV:3:TV-110", PACKET_VERSION, getMSTime());
            
                        datagramSocketSendto(revcAddress, response, strlen(response)+1);
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

//              usleep(1000*1000); //1s   Don't sleep because datagram recevie is blocked
        }
        return NULL;
}


/*
*  versionInfo process necessary???  (must follow phone)
*/
static pthread_t versionInfoThread;
static int versionInfoPort = 6550; //0x1996
void *versionInfoLoop(void *arg)
{
    
    int mSock = nsStreamSocketServerStart(versionInfoPort);

    if(INVALID_SOCKET == mSock)
    {
        return NULL;
    }

    int connectSock;
    unsigned int mAddrSize;
    struct sockaddr_in connectIPAddr;

    mAddrSize = sizeof(connectIPAddr);
    memset(&connectIPAddr, 0, mAddrSize);

    while(1)
    {
        connectSock = accept(mSock, (struct sockaddr*)&connectIPAddr, &mAddrSize);
        printf("versionInfoLoop connect ip = %s \n", inet_ntoa(connectIPAddr.sin_addr));
        if(connectSock != INVALID_SOCKET)
        {
            char ver[] = "Version 4";
            
            if(send(connectSock, ver, sizeof(ver), 0) > 0)
            {
                close(connectSock);
                connectSock = INVALID_SOCKET;
            }
        }
        else
        {
            perror("\n versionInfoLoop accept socket error: ");
            usleep(300*1000); // 300ms
        }
    }

    close(mSock);
    mSock = INVALID_SOCKET;
    
    return NULL;
}

void nsPPServiceInit(void)
{
    printf("--> [%s %s]\n", __DATE__, __TIME__);
        serviceMainThread = createThread(serviceMainLoop, NULL);
        connectMainThread = createThread(connectMainLoop, NULL);
    versionInfoThread = createThread(versionInfoLoop, NULL);
}
