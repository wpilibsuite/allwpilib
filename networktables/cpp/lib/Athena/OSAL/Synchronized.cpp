#include "OSAL/Synchronized.h"
#include <stdlib.h>


pthread_mutexattr_t mta;
bool hasInit = false;


NTSynchronized::NTSynchronized(NTReentrantSemaphore& semaphore):m_semaphore(semaphore)
{
	m_semaphore.take();
}

NTSynchronized::~NTSynchronized()
{
	m_semaphore.give();
}
