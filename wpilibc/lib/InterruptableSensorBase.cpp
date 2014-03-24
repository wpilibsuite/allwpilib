/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "InterruptableSensorBase.h"
#include "Utility.h"

InterruptableSensorBase::InterruptableSensorBase()
{
	m_interrupt = NULL;
}

InterruptableSensorBase::~InterruptableSensorBase()
{

}

void InterruptableSensorBase::AllocateInterrupts(bool watcher)
{
	wpi_assert(m_interrupt == NULL);
	// Expects the calling leaf class to allocate an interrupt index.
	int32_t status = 0;
	m_interrupt = initializeInterrupts(m_interruptIndex, watcher, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Cancel interrupts on this device.
 * This deallocates all the chipobject structures and disables any interrupts.
 */
void InterruptableSensorBase::CancelInterrupts()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	cleanInterrupts(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	m_interrupt = NULL;
}

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 */
void InterruptableSensorBase::WaitForInterrupt(float timeout)
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	waitForInterrupt(m_interrupt, timeout, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Enable interrupts to occur on this input.
 * Interrupts are disabled when the RequestInterrupt call is made. This gives time to do the
 * setup of the other options before starting to field interrupts.
 */
void InterruptableSensorBase::EnableInterrupts()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	enableInterrupts(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Disable Interrupts without without deallocating structures.
 */
void InterruptableSensorBase::DisableInterrupts()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	disableInterrupts(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Return the timestamp for the interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * @return Timestamp in seconds since boot.
 */
double InterruptableSensorBase::ReadInterruptTimestamp()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	double timestamp = readInterruptTimestamp(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return timestamp;
}
