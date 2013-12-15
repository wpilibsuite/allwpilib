/*
 * DefaultThreadManger.cpp
 *
 *  Created on: Sep 21, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/thread/DefaultThreadManager.h"
#include <stdio.h>


PeriodicNTThread::PeriodicNTThread(PeriodicRunnable* _r, const char* _name) : 
			name(_name), thread(new NTTask(name, (FUNCPTR)PeriodicNTThread::taskMain)), r(_r), run(true){
	fprintf(stdout, "Starting task: %s\n", name);
	fflush(stdout);
	thread->Start((UINT32)this);
}

PeriodicNTThread::~PeriodicNTThread(){
	//TODO somehow do this async
	//delete thread;
}
int PeriodicNTThread::taskMain(PeriodicNTThread* o){//static wrapper
	return o->_taskMain();
}
int PeriodicNTThread::_taskMain(){
	try {
		while(run){
			r->run();
		}
	} catch (...) {
		fprintf(stdout, "NTTask exited with uncaught exception %s\n", name);
		fflush(stdout);
		return 1;
	}
	fprintf(stdout, "NTTask exited normally: %s\n", name);
	fflush(stdout);
	return 0;
}
void PeriodicNTThread::stop() {
	run = false;
}
bool PeriodicNTThread::isRunning() {
	return thread->IsReady();
}

NTThread* DefaultThreadManager::newBlockingPeriodicThread(PeriodicRunnable* r, const char* name) {
	return new PeriodicNTThread(r, name);
}
