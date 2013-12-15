/*
 * ConnectionMonitorThread.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/connection/ConnectionMonitorThread.h"
#include "networktables2/connection/BadMessageException.h"
#include "networktables2/util/System.h"

ConnectionMonitorThread::ConnectionMonitorThread(ConnectionAdapter& _adapter, NetworkTableConnection& _connection) :
	adapter(_adapter), connection(_connection) {
}

void ConnectionMonitorThread::run() {
  
	if (adapter.keepAlive())
	{
		try{
			connection.read(adapter);
		} catch(BadMessageException& e){
			adapter.badMessage(e);
		} catch(IOException& e){
			adapter.ioException(e);
		}
	}
	else
	{
		sleep_ms(10);  //avoid busy-wait
		//Test to see this working properly
		//printf("--ConnectionMonitorThread::run Waiting to close\n");
	}
}

