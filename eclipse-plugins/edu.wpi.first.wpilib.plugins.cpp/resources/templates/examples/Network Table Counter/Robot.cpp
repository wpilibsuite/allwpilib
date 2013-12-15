//============================================================================
// Name        : Robot.cpp
// Author      :
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <networktables/NetworkTable.h>
#include <iostream>
using namespace std;

// This is a simple robot program
int main() {
	NetworkTable::SetServerMode();
	NetworkTable::SetTeam(190);
	NetworkTable* table = NetworkTable::GetTable("SmartDashboard");
	cout << "Started up" << endl;

	long i = 0;
	while (true) {
		cout << i << endl;
	    table->PutNumber("i", i);
	    i++;

	    sleep(1);
	}

	return 0;
}
