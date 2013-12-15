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

#if (defined __vxworks || defined WIN32)
#include "OSAL/Task.h"
#else
#include <pthread.h>
#endif

class DefaultThreadManager : public NTThreadManager{
public:
	virtual NTThread* newBlockingPeriodicThread(PeriodicRunnable* r, const char* name);
};

class PeriodicNTThread : public NTThread {
private:
#if (defined __vxworks || defined WIN32)
	const char* name;
	NTTask* thread;
#else
	pthread_t thread;
#endif
	PeriodicRunnable* r;
	bool run;
#if (defined __vxworks || defined WIN32)
	int _taskMain();
	static int taskMain(PeriodicNTThread* o);
#else//TODO make return int for pthread as well
	void _taskMain();
	static void* taskMain(PeriodicNTThread* o);
#endif
public:
	PeriodicNTThread(PeriodicRunnable* r, const char* name);
	virtual ~PeriodicNTThread();
	virtual void stop();
	virtual bool isRunning();
};

#endif /* DEFAULTTHREADMANAGER_H_ */
