/*
 * NscreenPrivateProtocolData.h
 *
 *  Created on: 2016-6-29
 *      Author: xiewx
 */

#ifndef NSCREENPRIVATEPROTOCOLDATA_H_
#define NSCREENPRIVATEPROTOCOLDATA_H_

#define BROADCAST_ADDR	"255.255.255.255"
#define BROADCAST_PORT	6537 // 0x1989
#define CONNECT_PORT	6553 // 0x1999

#define RECV_BUFFSIZE	1024

#ifndef INVALID_SOCKET
#define INVALID_SOCKET		-1
#endif

#define PACKET_SPLIT	":"
#define PACKET_VERSION	"1"
#define PACKET_SENDER_TYPE	"TV"
#define PACKET_SENDER_NAME	"TV-000" // ip last 3 bit


#define NPP_TRACE	printf("-->%s (%s line:%d)\n", __FUNCTION__, __FILE__, __LINE__)

/*PrivateProtocolPacket*/
typedef struct PPPacket
{
	char version[8];
	char packetNo[32]; //timestamp
	char senderName[64];
	char senderType[8];
	char commandNo[8];
	char expand[1024];
}stPPPacket;

#define PACKET_SIZE (sizeof(stPPPacket)+5)  // +5 separator  : : : : :

typedef enum
{
	EN_CMD_UNKOWN,
	EN_CMD_ONLINE,
	EN_CMD_OFFLINE,
	EN_CMD_ANSONLINE
}EN_COMMAND_NO;

#endif /* NSCREENPRIVATEPROTOCOLDATA_H_ */
