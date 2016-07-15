/*
 * DatagramSocket.h
 *
 *  Created on: 2016-7-14
 *      Author: xiewx
 */

#ifndef DATAGRAMSOCKET_H_
#define DATAGRAMSOCKET_H_

#include <iostream>
#include <string>
using namespace std;

class DatagramSocket
{
private:
	int mSock;
	unsigned int mPort;
	string mIP;

public:
	DatagramSocket(string ip, unsigned int port);
	virtual ~DatagramSocket();

	int recevie(string &ip,  unsigned int &port, char *data, unsigned int datasize);
	int send(string &data); // use ip and port  from init
	int sendto(string &ip,  unsigned int &port, string &data);
};

#endif /* DATAGRAMSOCKET_H_ */
