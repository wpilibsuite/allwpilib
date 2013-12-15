
#include "HAL/Watchdog.h"

#include "HAL/HAL.h"
#include "ChipObject.h"

typedef tWatchdog Watchdog;
const double kDefaultWatchdogExpiration = 0.5;

void* initializeWatchdog(int32_t *status) {
  Watchdog* watchdog = tWatchdog::create(status);
  setWatchdogExpiration(watchdog, kDefaultWatchdogExpiration, status);
  setWatchdogEnabled(watchdog, true, status);
  return watchdog;
}

void cleanWatchdog(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  setWatchdogEnabled(watchdog, false, status);
  delete watchdog;
}

/**
 * Throw the dog a bone.
 * 
 * When everything is going well, you feed your dog when you get home.
 * Let's hope you don't drive your car off a bridge on the way home...
 * Your dog won't get fed and he will starve to death.
 * 
 * By the way, it's not cool to ask the neighbor (some random task) to
 * feed your dog for you.  He's your responsibility!
 * 
 * @returns Returns the previous state of the watchdog before feeding it.
 */  
bool feedWatchdog(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  bool previous = getWatchdogEnabled(watchdog_pointer, status);
  watchdog->strobeFeed(status);
  return previous;
}

/**
 * Put the watchdog out of its misery.
 * 
 * Don't wait for your dying robot to starve when there is a problem.
 * Kill it quickly, cleanly, and humanely.
 */
void killWatchdog(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  watchdog->strobeKill(status);
}

/**
 * Read how long it has been since the watchdog was last fed.
 * 
 * @return The number of seconds since last meal.
 */
double getWatchdogLastFed(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  uint32_t timer = watchdog->readTimer(status);
  return timer / (kSystemClockTicksPerMicrosecond * 1e6);
}

/**
 * Read what the current expiration is.
 * 
 * @return The number of seconds before starvation following a meal (watchdog starves if it doesn't eat this often).
 */
double getWatchdogExpiration(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  uint32_t expiration = watchdog->readExpiration(status);
  return expiration / (kSystemClockTicksPerMicrosecond * 1e6);
}

/**
 * Configure how many seconds your watchdog can be neglected before it starves to death.
 * 
 * @param expiration The number of seconds before starvation following a meal (watchdog starves if it doesn't eat this often).
 */
void setWatchdogExpiration(void* watchdog_pointer, double expiration, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  watchdog->writeExpiration((uint32_t)(expiration * (kSystemClockTicksPerMicrosecond * 1e6)), status);
}

/**
 * Find out if the watchdog is currently enabled or disabled (mortal or immortal).
 * 
 * @return Enabled or disabled.
 */
bool getWatchdogEnabled(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  bool enabled = !watchdog->readImmortal(status);
  return enabled;
}

/**
 * Enable or disable the watchdog timer.
 * 
 * When enabled, you must keep feeding the watchdog timer to
 * keep the watchdog active, and hence the dangerous parts 
 * (motor outputs, etc.) can keep functioning.
 * When disabled, the watchdog is immortal and will remain active
 * even without being fed.  It will also ignore any kill commands
 * while disabled.
 * 
 * @param enabled Enable or disable the watchdog.
 */
void setWatchdogEnabled(void* watchdog_pointer, bool enabled, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  watchdog->writeImmortal(!enabled, status);
}

/**
 * Check in on the watchdog and make sure he's still kicking.
 * 
 * This indicates that your watchdog is allowing the system to operate.
 * It is still possible that the network communications is not allowing the
 * system to run, but you can check this to make sure it's not your fault.
 * Check IsSystemActive() for overall system status.
 * 
 * If the watchdog is disabled, then your watchdog is immortal.
 * 
 * @return Is the watchdog still alive?
 */
bool isWatchdogAlive(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  bool alive = watchdog->readStatus_Alive(status);
  return alive;
}

/**
 * Check on the overall status of the system.
 * 
 * @return Is the system active (i.e. PWM motor outputs, etc. enabled)?
 */
bool isWatchdogSystemActive(void* watchdog_pointer, int32_t *status) {
  Watchdog* watchdog = (Watchdog*) watchdog_pointer;
  bool alive = watchdog->readStatus_SystemActive(status);
  return alive;
}
