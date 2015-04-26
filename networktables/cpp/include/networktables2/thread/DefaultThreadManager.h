/*
 * DefaultThreadManager.h
 * Desktop
 *
 *  Created on: Sep 21, 2012
 *      Author: Mitchell Wills
 */

#ifndef DEFAULTTHREADMANAGER_H_
#define DEFAULTTHREADMANAGER_H_


class DefaultThreadManager;
class PeriodicNTThread;

#include "networktables2/thread/PeriodicRunnable.h"
#include "networktables2/thread/NTThreadManager.h"
#include "networktables2/thread/NTThread.h"

#include <pthread.h>

class DefaultThreadManager : public NTThreadManager{
public:
	virtual NTThread* newBlockingPeriodicThread(PeriodicRunnable* r, const char* name);
};

class PeriodicNTThread : public NTThread {
private:
	pthread_t thread;
	PeriodicRunnable* r;
	bool run;
	void _taskMain();
	static void* taskMain(PeriodicNTThread* o);
public:
	PeriodicNTThread(PeriodicRunnable* r, const char* name);
	virtual ~PeriodicNTThread();
	virtual void stop();
	virtual bool isRunning();
};

#endif /* DEFAULTTHREADMANAGER_H_ */
