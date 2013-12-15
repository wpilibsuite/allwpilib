
#include "HAL/Utilities.h"

#include "HAL/HAL.h"
#include "ChipObject.h"

#include <usrLib.h> // for taskDelay
#include <sysLib.h> // for sysClkRateGet

const int32_t HAL_NO_WAIT = NO_WAIT;
const int32_t HAL_WAIT_FOREVER = WAIT_FOREVER;

void delayTicks(int32_t ticks) {
  taskDelay(ticks);
}

void delayMillis(double ms) {
  taskDelay((int32_t)((double)sysClkRateGet() * ms / 1000));
}

void delaySeconds(double s) {
  taskDelay((int32_t)((double)sysClkRateGet() * s));
}
