/*
 * main.c
 *
 *  Created on: 2016-6-30
 *      Author: xiewx
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "NsPPService.h"
#include "DatagramSocket.h"

int main(int argc, char *argv[]) {
	printf("!!!Hello World!!! \n");

	nsPPServiceInit();
	while(1){
		usleep(100*1000);
	}
	return 0;
}

