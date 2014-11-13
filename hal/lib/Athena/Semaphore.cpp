#include "HAL/Semaphore.hpp"

#include "Log.hpp"

// set the logging level
TLogLevel semaphoreLogLevel = logDEBUG;

#define SEMAPHORE_LOG(level) \
    if (level > semaphoreLogLevel) ; \
    else Log().Get(level)

// See: http://www.vxdev.com/docs/vx55man/vxworks/ref/semMLib.html
const uint32_t SEMAPHORE_Q_FIFO= 0x01; // TODO: Support
const uint32_t SEMAPHORE_Q_PRIORITY = 0x01; // TODO: Support
const uint32_t SEMAPHORE_DELETE_SAFE = 0x04; // TODO: Support
const uint32_t SEMAPHORE_INVERSION_SAFE = 0x08; // TODO: Support

const int32_t SEMAPHORE_NO_WAIT = 0;
const int32_t SEMAPHORE_WAIT_FOREVER = -1;

const uint32_t SEMAPHORE_EMPTY = 0;
const uint32_t SEMAPHORE_FULL = 1;

MUTEX_ID initializeMutexRecursive()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  MUTEX_ID sem = new pthread_mutex_t();
  pthread_mutex_init(sem, &attr);
  pthread_mutexattr_destroy(&attr);
  return sem;
}

MUTEX_ID initializeMutexNormal()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  MUTEX_ID sem = new pthread_mutex_t();
  pthread_mutex_init(sem, &attr);
  pthread_mutexattr_destroy(&attr);
  return sem;
}

void deleteMutex(MUTEX_ID sem)
{
	pthread_mutex_destroy(sem);
	delete sem;
}

/**
 * Lock the semaphore, blocking until it's available.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t takeMutex(MUTEX_ID sem)
{
    return pthread_mutex_lock(sem);
}

int8_t tryTakeMutex(MUTEX_ID sem)
{
    return pthread_mutex_trylock(sem);
}

/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveMutex(MUTEX_ID sem)
{
	return pthread_mutex_unlock(sem);
}

SEMAPHORE_ID initializeSemaphore(uint32_t initial_value) {
  SEMAPHORE_ID sem = new sem_t;
  sem_init(sem, 0, initial_value);
  return sem;
}

void deleteSemaphore(SEMAPHORE_ID sem) {
  sem_destroy(sem);
  delete sem;
}

/**
 * Lock the semaphore, blocking until it's available.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t takeSemaphore(SEMAPHORE_ID sem)
{
    return sem_wait(sem);
}

int8_t tryTakeSemaphore(SEMAPHORE_ID sem)
{
    return sem_trywait(sem);
}

/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveSemaphore(SEMAPHORE_ID sem)
{
  return sem_post(sem);
}


MULTIWAIT_ID initializeMultiWait() {
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  MULTIWAIT_ID cond = new pthread_cond_t();
  pthread_cond_init(cond, &attr);
  pthread_condattr_destroy(&attr);
  return cond;
}

void deleteMultiWait(MULTIWAIT_ID sem) {
  pthread_cond_destroy(sem);
  delete sem;
}

int8_t takeMultiWait(MULTIWAIT_ID sem, MUTEX_ID m, int32_t timeout) {
  takeMutex(m);
  int8_t val = pthread_cond_wait(sem, m);
  giveMutex(m);
  return val;
}

int8_t giveMultiWait(MULTIWAIT_ID sem) {
  return pthread_cond_broadcast(sem);
}


