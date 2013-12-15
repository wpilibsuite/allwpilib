/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "OSAL/Synchronized.h"

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

NTSynchronized::NTSynchronized(NTReentrantSemaphore& sem)
{
	usingSem = true;
	m_sem = &sem;
	m_sem->take();
}

/**
 * Syncronized destructor.
 * This destructor frees the semaphore ensuring that the resource is freed for the block
 * containing the Synchronized object.
 */
NTSynchronized::~NTSynchronized()
{
	if(usingSem)
		m_sem->give();
	else
		semGive(m_semaphore);
}
