//============================================================================
// Name        : dlnaPPCPPlus.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "PrivateProtocolService.h"
#include <unistd.h>
#include <iostream>
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	nsPrivateProtocolServiceInit();
	while(1)
	{
			usleep(100*1000);
		}
	return 0;
}
