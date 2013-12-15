
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H

extern "C" {
  extern const double kDefaultWatchdogExpiration;
  
  void* initializeWatchdog(int32_t *status);
  void cleanWatchdog(void* watchdog_pointer, int32_t *status);
  
  bool feedWatchdog(void* watchdog_pointer, int32_t *status);
  void killWatchdog(void* watchdog_pointer, int32_t *status);
  double getWatchdogLastFed(void* watchdog_pointer, int32_t *status);
  double getWatchdogExpiration(void* watchdog_pointer, int32_t *status);
  void setWatchdogExpiration(void* watchdog_pointer, double expiration, int32_t *status);
  bool getWatchdogEnabled(void* watchdog_pointer, int32_t *status);
  void setWatchdogEnabled(void* watchdog_pointer, bool enabled, int32_t *status);
  bool isWatchdogAlive(void* watchdog_pointer, int32_t *status);
  bool isWatchdogSystemActive(void* watchdog_pointer, int32_t *status);
}
#endif
