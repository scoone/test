/*
 * PrivateProtocolService.cpp
 *
 *  Created on: 2016-7-14
 *      Author: xx
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "DatagramSocket.h"
#include "StreamSocket.h"
#include "PrivateProtocolData.h"
#include "PrivateProtocolService.h"

#include <vector>
using namespace std;

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

static int streamSocketCommandProcess(StreamSocket &ssocket, char *command)
{
	if('\0' == command[0]) //?????????
	{
		return 0xff;
	}

	StreamPrivatePacket spp;
	vector<char *> strvec = spp.parse(command, STREAM_PACKET_SPLIT);

	if(strvec.size() <= 0)
	{
		cerr<<"streamSocketCommandProcess strvec.size() <= 0" <<endl;
		return 0xfe;
	}

	int ret = 0;
	string response;

    switch(atoi(strvec[0]))
    {
        case EN_CMD_ISONLINE:
        	response=strvec[0];
        	response += ">>";
        	response += ONLINE;
        	cout << response << endl;
        	//ret = ssocket.send(ssocket.getRemoteSocket(), response);
            break;
        case EN_CMD_MEDIA_PLAY:
        	cout << "EN_CMD_MEDIA_PLAY"<< endl;
        	break;
        case EN_CMD_KEY:
        	cout << "EN_CMD_KEY"<< endl;
        	//deal with key
        	break;
        case EN_CMD_MUSE:
        {
        	cout << "EN_CMD_MUSE"<< endl;
        	//deal with mouse
        }
        	break;
        case EN_CMD_INPUTSTR:
        	cout << "EN_CMD_INPUTSTR"<< endl;
        	break;
        default:
        	 cout<<"address is" << ssocket.getRemoteIP();
        	 cout<<":" << ssocket.getRemotePort()<< endl;
        	 cout <<"command  *" <<command << endl;
        	 cout<< strvec.size()<<"@  " << strvec[0]<< endl;
            break;
    }

    return ret;
}

static void* recvDataLoop(void *arg)
{
	StreamSocket *ss =(StreamSocket *)arg;

	if(NULL == ss)
	{
		cerr << "recvDataLoop arg is NULL"<<__LINE__<<endl;
		return NULL;
	}
	cout <<"###@@@enter recvDataLoop@@@@### ip:"<< ss->getRemoteIP()<<":"<< ss->getRemotePort()<<endl;
	StreamSocket sock = StreamSocket(ss->getRemoteSocket(), ss->getRemoteIP(), ss->getRemotePort());

    char buff[RECV_BUFFSIZE];
    int len;

    while(1)
    {
    	len = sock.receive(sock.getRemoteSocket(), buff, sizeof(buff), 45);
    	if(len > 0)
    	{
    		  if(-1 == streamSocketCommandProcess(sock, buff))
				{
					cout<<"\n nsStreamSocketCommandProcess send fail!:"<<endl;
//					break;
				}
    	}
    }

    cerr<<"\n###########@@@@@@@@@ exit @@@@@@@@############### \n";
    sock.close(sock.getRemoteSocket());
    return NULL;
}

void *connectMainLoop(void *arg)
{
	StreamSocket *socket = new StreamSocket();

	if(socket)
	{
		int connectSock;
		string connectIP;
		unsigned int port;
		int ret = socket->startServer(CONNECT_PORT);

		while(ret >= 0)
		{
			connectSock = socket->accept(connectIP, port);
			if(connectSock != INVALID_SOCKET)
			{
				cout <<"connect ip ="<<connectIP<<":"<<port<<endl;
				socket->setRemoteSocket(connectSock, connectIP,  port);
				createThread(recvDataLoop, (void*)socket);
			}
			else
			{
				perror("\n connectMainLoop accept socket error: ");
				usleep(300*1000); // 300ms
			}
		}

		delete(socket);
		socket = NULL;
	}

	return NULL;
}


void datagramCommandProcess(DatagramSocket *sock, string &recvIP, unsigned int &port, string &msg)
{
	switch(atoi(msg.c_str()))
	{
		case EN_CMD_ONLINE:
		{
			string response ="1:1468035052270:TV-104:TV:3:TV-104";
			sock->sendto(recvIP, port, response);
		}
		break;

		default:
			break;
	}
}

void *serviceMainLoop(void *arg)
{
	char msg[RECV_BUFFSIZE];
	DatagramSocket *datagramSock = new DatagramSocket(BROADCAST_ADDR, BROADCAST_PORT);
	DatagramPrivatePacket *datagreampacket = new DatagramPrivatePacket();
	string recvFrom;
	unsigned int port;

	while(datagramSock)
	{
		if(datagramSock->recevie(recvFrom, port, msg, RECV_BUFFSIZE) > 0)
		{
			if(datagreampacket)
			{
				datagreampacket->parse(msg);
				datagramCommandProcess(datagramSock, recvFrom, port, datagreampacket->getCommandNo());
			}
		}

//		usleep(1000*1000); //1s   Don't sleep because datagram recevie is blocked
	}

	if(datagramSock)
	{
		delete(datagramSock);
		datagramSock = NULL;
	}

	if(datagreampacket)
	{
		delete(datagreampacket);
		datagreampacket = NULL;
	}

	return NULL;
}

void nsPrivateProtocolServiceInit(void)
{
	serviceMainThread = createThread(serviceMainLoop, NULL);
	connectMainThread = createThread(connectMainLoop, NULL);
}
