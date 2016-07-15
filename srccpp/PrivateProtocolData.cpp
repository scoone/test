/*
 * PrivateProtocolData.cpp
 *
 *  Created on: 2016-7-15
 *      Author: xx
 */
#include <iostream>
#include <string>
using namespace std;

#include <string.h>
#include "PrivateProtocolData.h"

/**======================
 * DatagramPrivatePacket
 * =======================
 */
void DatagramPrivatePacket::parse(char *data)
{
    char * s = NULL;

    mversion.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
    {
    	mversion = s;
    }

    mpacketNo.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
	{
		mpacketNo =s;
	}

    msenderName.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
	{
    	msenderName = s;
	}

    msenderType.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
	{
    	msenderType = s;
	}

    mcommandNo.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
	{
    	mcommandNo = s;
	}

    mexpand.clear();
    if((s = strsep(&data, DATAGRAM_PACKET_SPLIT)))
	{
    	mexpand = s;
	}
}

string &DatagramPrivatePacket::getVersion()
{
	return mversion;
}

string &DatagramPrivatePacket::getPacketNo()
{
	return mpacketNo;
}

string &DatagramPrivatePacket::getName()
{
	return msenderName;
}

string &DatagramPrivatePacket::getType()
{
	return msenderType;
}

string &DatagramPrivatePacket::getCommandNo()
{
	return mcommandNo;
}

string &DatagramPrivatePacket::getExpand()
{
	return mexpand;
}


/**======================
 * StreamPrivatePacket
 * =======================
 */
StreamPrivatePacket::StreamPrivatePacket()
{
}

StreamPrivatePacket::~StreamPrivatePacket()
{
}
vector<char *> &StreamPrivatePacket::parse(char *data, const char *splitstr)
{
	/* use C function */
	mcstrvec.clear();
	char *str = data;
	char * s = strsep(&str, splitstr);

	if(s)
	{
		mcstrvec.push_back(s);
	}

	do
	{
		if((s = strsep(&str, splitstr)))
		{
			if(s[0] != '\0')
			{
				mcstrvec.push_back(s);
			}
		}
	}while(str);
//
//	for(unsigned int i = 0; i < mcstrvec.size(); i++)
//	{
//		cout <<i<<" "<< mcstrvec[i]<<endl;
//	}

	return mcstrvec;
}

vector<string> &StreamPrivatePacket::parse(string &data, string &splitstr)
{
	mstrvec.clear();

	string strtemp;
	string::size_type pos1 = 0;
	string::size_type pos2;

	pos2 = data.find(splitstr);

	while (string::npos != pos2)
	{
		mstrvec.push_back(data.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = data.find(splitstr, pos1);
	}
	mstrvec.push_back(data.substr(pos1));

	vector<string>::iterator iter1 = mstrvec.begin(), iter2 = mstrvec.end();
	while (iter1 != iter2)
	{
			cout << *iter1 << endl;
			++iter1;
	}

	return mstrvec;
}
