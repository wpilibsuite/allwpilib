/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "InterruptableSensorBase.h"
#include "Utility.h"

Resource *InterruptableSensorBase::m_interrupts = NULL;

InterruptableSensorBase::InterruptableSensorBase()
{
	m_interrupt = NULL;
	Resource::CreateResourceObject(&m_interrupts, interrupt_kNumSystems);
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
	m_interrupts->Free(m_interruptIndex);
}

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 * @param ignorePrevious If true, ignore interrupts that happened before
 * WaitForInterrupt was called.
 * @return What interrupts fired
 */
InterruptableSensorBase::WaitResult InterruptableSensorBase::WaitForInterrupt(float timeout, bool ignorePrevious)
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	uint32_t result;

	result = waitForInterrupt(m_interrupt, timeout, ignorePrevious, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	return static_cast<WaitResult>(result);
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
 * Return the timestamp for the rising interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * The rising-edge interrupt should be enabled with
 * {@link #DigitalInput.SetUpSourceEdge}
 * @return Timestamp in seconds since boot.
 */
double InterruptableSensorBase::ReadRisingTimestamp()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	double timestamp = readRisingTimestamp(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return timestamp;
}

/**
 * Return the timestamp for the falling interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * The falling-edge interrupt should be enabled with
 * {@link #DigitalInput.SetUpSourceEdge}
 * @return Timestamp in seconds since boot.
*/
double InterruptableSensorBase::ReadFallingTimestamp()
{
	wpi_assert(m_interrupt != NULL);
	int32_t status = 0;
	double timestamp = readFallingTimestamp(m_interrupt, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return timestamp;
}
