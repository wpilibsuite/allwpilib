/*
 * NTThreadManager.h
 *
 *  Created on: Sep 21, 2012
 *      Author: Mitchell Wills
 */

#ifndef NTTHREADMANAGER_H_
#define NTTHREADMANAGER_H_

class NTThreadManager;

#include "networktables2/thread/NTThread.h"
#include "networktables2/thread/PeriodicRunnable.h"

/**
 * A thread manager that can be used to obtain new threads
 * 
 * @author Mitchell
 *
 */
class NTThreadManager
{
public:
	virtual ~NTThreadManager()
	{
	}
	/**
	 * @param r
	 * @param name the name of the thread
	 * @return a thread that will run the provided runnable repeatedly with the assumption that the runnable will block
	 */
	virtual NTThread* newBlockingPeriodicThread(PeriodicRunnable* r, const char* name) = 0;
};

#endif /* NTTHREADMANAGER_H_ */
