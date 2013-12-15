
#include "HAL/Utilities.h"

#include "HAL/HAL.h"
#include "ChipObject.h"

const int32_t HAL_NO_WAIT = 0;
const int32_t HAL_WAIT_FOREVER = -1;

void delayTicks(int32_t ticks) {
  struct timespec test, remaining;
  test.tv_sec = 0;
  test.tv_nsec = ticks * 3;
  nanosleep(&test, &remaining);
}

void delayMillis(double ms) {
  struct timespec test, remaining;
  test.tv_sec = ms/1000;
  test.tv_nsec = 1000*(((uint64_t) ms)%1000000);
  nanosleep(&test, &remaining);
}

void delaySeconds(double s) {
  struct timespec test, remaining;
  test.tv_sec = 0;
  test.tv_nsec = s * 1000000000.0;
  nanosleep(&test, &remaining);
}
