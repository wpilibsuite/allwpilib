/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Watchdog.h"

/**
 * The Watchdog is born.
 */
Watchdog::Watchdog()
	:	m_watchdog(NULL)
{
	int32_t status = 0;
	m_watchdog = initializeWatchdog(&status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Time to bury him in the back yard.
 */
Watchdog::~Watchdog()
{
	int32_t status = 0;
	cleanWatchdog(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	m_watchdog = NULL;
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
bool Watchdog::Feed()
{
	int32_t status = 0;
	bool previous = feedWatchdog(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return previous;
}

/**
 * Put the watchdog out of its misery.
 * 
 * Don't wait for your dying robot to starve when there is a problem.
 * Kill it quickly, cleanly, and humanely.
 */
void Watchdog::Kill()
{
	int32_t status = 0;
	killWatchdog(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read how long it has been since the watchdog was last fed.
 * 
 * @return The number of seconds since last meal.
 */
double Watchdog::GetTimer()
{
	int32_t status = 0;
	double timer = getWatchdogLastFed(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return timer;
}

/**
 * Read what the current expiration is.
 * 
 * @return The number of seconds before starvation following a meal (watchdog starves if it doesn't eat this often).
 */
double Watchdog::GetExpiration()
{
	int32_t status = 0;
	bool expiration = getWatchdogExpiration(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return expiration;
}

/**
 * Configure how many seconds your watchdog can be neglected before it starves to death.
 * 
 * @param expiration The number of seconds before starvation following a meal (watchdog starves if it doesn't eat this often).
 */
void Watchdog::SetExpiration(double expiration)
{
	int32_t status = 0;
	setWatchdogExpiration(m_watchdog, expiration, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Find out if the watchdog is currently enabled or disabled (mortal or immortal).
 * 
 * @return Enabled or disabled.
 */
bool Watchdog::GetEnabled()
{
	int32_t status = 0;
	bool enabled = getWatchdogEnabled(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
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
void Watchdog::SetEnabled(bool enabled)
{
	int32_t status = 0;
	setWatchdogEnabled(m_watchdog, enabled, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
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
bool Watchdog::IsAlive()
{
	int32_t status = 0;
	bool alive = isWatchdogAlive(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return alive;
}

/**
 * Check on the overall status of the system.
 * 
 * @return Is the system active (i.e. PWM motor outputs, etc. enabled)?
 */
bool Watchdog::IsSystemActive()
{
	int32_t status = 0;
	bool alive = isWatchdogSystemActive(m_watchdog, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return alive;
}

