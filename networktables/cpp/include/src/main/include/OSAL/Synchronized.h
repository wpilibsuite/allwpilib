/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef NT_SYNCHRONIZED_H
#define NT_SYNCHRONIZED_H

#define NT_CRITICAL_REGION(s) { NTSynchronized _sync(s);
#define NT_END_REGION }

#if (defined __vxworks || defined WIN32)

#ifdef __vxworks
#include <vxWorks.h>
#endif
#include <semLib.h>

class NTReentrantSemaphore
{
public:
	explicit NTReentrantSemaphore(){
		m_semaphore = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
	};
	~NTReentrantSemaphore(){
		semDelete(m_semaphore);
	};
	void take(){
		semTake(m_semaphore, WAIT_FOREVER);
	};
	void give(){
		semGive(m_semaphore);
	};
private:
	SEM_ID m_semaphore;
};

#else

#include <pthread.h>

class NTReentrantSemaphore
{
public:
	explicit NTReentrantSemaphore(){
		pthread_mutexattr_init(&mta);
		pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_semaphore, &mta);
	};
	~NTReentrantSemaphore(){
		pthread_mutex_unlock(&m_semaphore);
		pthread_mutex_destroy(&m_semaphore);
	};
	void take(){
		pthread_mutex_lock(&m_semaphore);
	};
	void give(){
		pthread_mutex_unlock(&m_semaphore);
	};
private:
	pthread_mutexattr_t mta;
	pthread_mutex_t m_semaphore;
};
#endif // __vxworks

/**
 * Provide easy support for critical regions.
 * A critical region is an area of code that is always executed under mutual exclusion. Only
 * one task can be executing this code at any time. The idea is that code that manipulates data
 * that is shared between two or more tasks has to be prevented from executing at the same time
 * otherwise a race condition is possible when both tasks try to update the data. Typically
 * semaphores are used to ensure only single task access to the data.
 * Synchronized objects are a simple wrapper around semaphores to help ensure that semaphores
 * are always signaled (semGive) after a wait (semTake).
 */
class NTSynchronized
{
public:
	explicit NTSynchronized(NTReentrantSemaphore&);
	//TODO remove vxworks SEM_ID support
#if (defined __vxworks || defined WIN32)
	explicit NTSynchronized(SEM_ID);
#endif
	virtual ~NTSynchronized();
private:
#if (defined __vxworks || defined WIN32)
	bool usingSem;
	NTReentrantSemaphore* m_sem;
	SEM_ID m_semaphore;
#else
	NTReentrantSemaphore& m_semaphore;
#endif
};



#endif
