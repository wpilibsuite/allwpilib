
/*
* System.cpp
*
*  Created on: Sep 26, 2012
*      Author: Mitchell Wills
*/

#include "windows.h"
#include "networktables2/util/System.h"
//#include "semLib.h"
#include <stdio.h>
//#include <sysLib.h> // for sysClkRateGet
//#include <usrLib.h> // for taskDelay
//timeGetTime() uses Winmm.lib 
#pragma comment (lib,"Winmm.lib")
#pragma comment( lib, "Ws2_32" )
void sleep_ms(unsigned long ms){
	//taskDelay((INT32)((double)sysClkRateGet() * ms / 1000));
	Sleep(ms);
}
unsigned long currentTimeMillis(){
	//struct timespec tp;
	//clock_gettime(CLOCK_REALTIME,&tp);
	DWORD dwTick_ = timeGetTime();
	//return tp.tv_sec*1000 + tp.tv_nsec/1000;
	return (long)dwTick_;
}
void writeWarning(const char* message){
	fprintf(stderr, "%s\n", message);
	fflush(stderr);
	//TODO implement write warning with wpilib error stuff
}
