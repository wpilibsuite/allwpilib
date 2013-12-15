/*
 * DefaultThreadManager.cpp
 * Desktop
 * Author: Mitchell Wills
 *
 */

#include "networktables2/thread/DefaultThreadManager.h"
#include <exception>
#include <signal.h>
#include <stdio.h>


PeriodicNTThread::PeriodicNTThread(PeriodicRunnable* _r, const char* name) : r(_r), run(true){
  if(pthread_create(&thread, NULL, (void* (*)(void*))PeriodicNTThread::taskMain, (void*)this))
    throw std::exception();
}
PeriodicNTThread::~PeriodicNTThread(){
  stop();
  if(thread == pthread_self()){
    fprintf(stderr, "WARNING: thread destructor called from this thread\n");
  }
  pthread_join(thread, NULL);
}
void* PeriodicNTThread::taskMain(PeriodicNTThread* o){//static wrapper
	o->_taskMain();
	return 0;
}
void PeriodicNTThread::_taskMain(){
	while(run){
		r->run();
	}
}
void PeriodicNTThread::stop() {
	run = false;
	//pthread_cancel(thread);
}
bool PeriodicNTThread::isRunning() {
  return pthread_kill(thread, 0) == 0;
}

NTThread* DefaultThreadManager::newBlockingPeriodicThread(PeriodicRunnable* r, const char* name) {
	return new PeriodicNTThread(r, name);
}
