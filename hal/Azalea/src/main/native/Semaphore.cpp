
#include "HAL/Semaphore.h"

#include "ChipObject.h"

const uint32_t SEMAPHORE_Q_FIFO = SEM_Q_FIFO;
const uint32_t SEMAPHORE_Q_PRIORITY = SEM_Q_PRIORITY;
const uint32_t SEMAPHORE_DELETE_SAFE = SEM_DELETE_SAFE;
const uint32_t SEMAPHORE_INVERSION_SAFE = SEM_INVERSION_SAFE;

const int32_t SEMAPHORE_NO_WAIT = NO_WAIT;
const int32_t SEMAPHORE_WAIT_FOREVER = WAIT_FOREVER;

const uint32_t SEMAPHORE_EMPTY = SEM_EMPTY;
const uint32_t SEMAPHORE_FULL = SEM_FULL;

MUTEX_ID initializeMutex(uint32_t flags) {
	return semMCreate(flags);
}

void deleteMutex(MUTEX_ID sem) {
	semDelete(sem);
}

/**
 * Lock the semaphore, blocking until it's available.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t takeMutex(MUTEX_ID sem, int32_t timeout) {
	return semTake(sem, timeout);
}
/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveMutex(MUTEX_ID sem) {
	return semGive(sem);
}


SEMAPHORE_ID initializeSemaphore(uint32_t flags, uint32_t initial_state) {
	return semBCreate(flags, (SEM_B_STATE) initial_state);
}

void deleteSemaphore(SEMAPHORE_ID sem) {
	semDelete(sem);
}

/**
 * Lock the semaphore, blocking until it's available.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t takeSemaphore(SEMAPHORE_ID sem, int32_t timeout) {
	return semTake(sem, timeout);
}
/**
 * Unlock the semaphore.
 * @return 0 for success, -1 for error. If -1, the error will be in errno.
 */
int8_t giveSemaphore(SEMAPHORE_ID sem) {
	return semGive(sem);
}


MULTIWAIT_ID initializeMultiWait() {
	return initializeSemaphore(SEMAPHORE_Q_PRIORITY, SEMAPHORE_FULL);
}

void deleteMultiWait(MULTIWAIT_ID sem) {
	deleteSemaphore(sem);
}

int8_t takeMultiWait(MULTIWAIT_ID sem, int32_t timeout) {
	return takeSemaphore(sem, timeout);
}

int8_t giveMultiWait(MULTIWAIT_ID sem) {
	return semFlush(sem);
}
