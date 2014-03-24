/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "OSAL/Synchronized.h"

//TODO see what the STATUS is suppose to return for success
STATUS 	  semGive 	(SEM_ID semId)
{
	::LeaveCriticalSection( semId );
	return 0;
}
STATUS 	  semTake 	(SEM_ID semId, int timeout)
{
	if (timeout==WAIT_FOREVER)
		::EnterCriticalSection( semId );
	else
	{
		BOOL result;
		int TimeOut=0;
		do 
		{
			result=::TryEnterCriticalSection( semId );
			if (result==0)
				Sleep(10);
		} while ((result==0)&&(TimeOut++<timeout));
		assert(result!=0);  //TODO timeout
	}
	return 0;
}

SEM_ID 	  semMCreate 	(int options)
{
	SEM_ID ret=new CRITICAL_SECTION;
	::InitializeCriticalSection( ret );
	return ret;
}

STATUS 	  semDelete 	(SEM_ID semId)
{
	::DeleteCriticalSection( semId );
	delete semId;
	return 0;
}


/**
 * Synchronized class deals with critical regions.
 * Declare a Synchronized object at the beginning of a block. That will take the semaphore.
 * When the code exits from the block it will call the destructor which will give the semaphore.
 * This ensures that no matter how the block is exited, the semaphore will always be released.
 * Use the CRITICAL_REGION(SEM_ID) and END_REGION macros to make the code look cleaner (see header file)
 * @param semaphore The semaphore controlling this critical region.
 */
NTSynchronized::NTSynchronized(SEM_ID semaphore)
{
	usingSem = false;
	m_semaphore = semaphore;
	semTake(m_semaphore, WAIT_FOREVER);
}

NTSynchronized::NTSynchronized(NTReentrantSemaphore& semaphore)
{
	usingSem = true;
	m_sem = &semaphore;
	m_sem->take();
}

/**
 * This destructor unlocks the semaphore.
 */
NTSynchronized::~NTSynchronized()
{
	if(usingSem)
		m_sem->give();
	else
		semGive(m_semaphore);
}
