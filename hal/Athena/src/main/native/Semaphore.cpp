
#include "HAL/Semaphore.h"

#include "ChipObject.h"

// See: http://www.vxdev.com/docs/vx55man/vxworks/ref/semMLib.html
const uint32_t SEMAPHORE_Q_FIFO= 0x01; // TODO: Support
const uint32_t SEMAPHORE_Q_PRIORITY = 0x01; // TODO: Support
const uint32_t SEMAPHORE_DELETE_SAFE = 0x04; // TODO: Support
const uint32_t SEMAPHORE_INVERSION_SAFE = 0x08; // TODO: Support

const int32_t SEMAPHORE_NO_WAIT = 0;
const int32_t SEMAPHORE_WAIT_FOREVER = -1;

const uint32_t SEMAPHORE_EMPTY = 0;
const uint32_t SEMAPHORE_FULL = 1;

MUTEX_ID initializeMutex(uint32_t flags) {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  MUTEX_ID sem = new pthread_mutex_t();
  pthread_mutex_init(sem, &attr);
  pthread_mutexattr_destroy(&attr);
  return sem;
}

void deleteMutex(MUTEX_ID sem) {
  pthread_mutex_destroy(sem);
  delete sem;
}

/**
 * Lock the semaphore, blocking until it's available.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t takeMutex(MUTEX_ID sem, int32_t timeout) {
  if (timeout == SEMAPHORE_NO_WAIT) {
    return pthread_mutex_trylock(sem);
  } else if (timeout == SEMAPHORE_WAIT_FOREVER) {
    return pthread_mutex_lock(sem);
  } else {
    // struct timespec test;
    // return pthread_mutex_timedlock(sem, );
    return -1; // TODO: implement timed wait
  }
}

/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveMutex(MUTEX_ID sem) {
  // return semGive(sem);
  // return sem_post(sem);
  return pthread_mutex_unlock(sem);
}

SEMAPHORE_ID initializeSemaphore(uint32_t flags, uint32_t initial_value) {
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
int8_t takeSemaphore(SEMAPHORE_ID sem, int32_t timeout) {
  if (timeout == SEMAPHORE_NO_WAIT) {
    return sem_trywait(sem);
  } else if (timeout == SEMAPHORE_WAIT_FOREVER) {
    return sem_wait(sem);
  } else {
    // return sem_timedwait(sem, );
    return -1; // TODO: implement timed wait
  }
}

/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveSemaphore(SEMAPHORE_ID sem) {
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

int8_t takeMultiWait(MULTIWAIT_ID sem, int32_t timeout) {
  MUTEX_ID m = initializeMutex(NULL);
  takeMutex(m, SEMAPHORE_WAIT_FOREVER);
  int8_t val = pthread_cond_wait(sem, m);
  deleteMutex(m);
  return val;
}

int8_t giveMultiWait(MULTIWAIT_ID sem) {
  return pthread_cond_broadcast(sem);
}


