/*
 * DatagramSocket.h
 *
 *  Created on: 2016-6-29
 *      Author: xiewx
 */

#ifndef DATAGRAMSOCKET_H_
#define DATAGRAMSOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct address_t
{
	unsigned int port;
	char ip[16];
}stAddress;

int datagramSocketInit(const char *address, int port);  // address ip[16]
int datagramSocketTerminate();
int datagramSocketRecevie(stAddress *address, char *buffer, unsigned int buffersize);
int datagramSocketSend(char *buffer, unsigned int buffersize);
int datagramSocketSendto(const stAddress *address, char *buffer, unsigned int buffersize);

#ifdef __cplusplus
}
#endif

#endif /* DATAGRAMSOCKET_H_ */
