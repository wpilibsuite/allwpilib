/*
 * System.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/util/System.h"
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

void sleep_ms(unsigned long ms){
	while(ms>500){
		usleep(500*1000);
		ms-=500;
	}
	usleep(ms*1000);
}
unsigned long currentTimeMillis(){//TODO improve
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0){
		return -1;
	}
	return ((tv.tv_sec % 86400) * 1000 + tv.tv_usec / 1000);
}
void writeWarning(const char* message){
	std::cerr << message;
}
