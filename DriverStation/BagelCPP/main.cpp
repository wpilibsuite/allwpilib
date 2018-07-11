#include "ds_comms.hpp"
#include <unistd.h>

int main (int argc, char * const argv[]) {
	
	Sim::DriverStationComms::start();

	while (true) {
		
		Sim::DriverStationComms::periodic_update();
		sleep(.02);
	}
    	return 0;
}
