/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "AnalogTriggerOutput.h"
#include "AnalogTrigger.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"

/**
 * Create an object that represents one of the four outputs from an analog trigger.
 *
 * Because this class derives from DigitalSource, it can be passed into routing functions
 * for Counter, Encoder, etc.
 *
 * @param trigger A pointer to the trigger for which this is an output.
 * @param outputType An enum that specifies the output on the trigger to represent.
 */
AnalogTriggerOutput::AnalogTriggerOutput(AnalogTrigger *trigger, AnalogTriggerType outputType)
	: m_trigger (trigger)
	, m_outputType (outputType)
{
	HALReport(HALUsageReporting::kResourceType_AnalogTriggerOutput, trigger->GetIndex(), outputType);
}

AnalogTriggerOutput::~AnalogTriggerOutput()
{
	if (m_interrupt != NULL)
	{
		int32_t status = 0;
		cleanInterrupts(m_interrupt, &status);
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
		m_interrupt = NULL;
		m_interrupts->Free(m_interruptIndex);
	}
}

/**
 * Get the state of the analog trigger output.
 * @return The state of the analog trigger output.
 */
bool AnalogTriggerOutput::Get()
{
  int32_t status = 0;
  bool result = getAnalogTriggerOutput(m_trigger->m_trigger, m_outputType, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return result;
}

/**
 * @return The value to be written to the channel field of a routing mux.
 */
uint32_t AnalogTriggerOutput::GetChannelForRouting()
{
	return (m_trigger->m_index << 2) + m_outputType;
}

/**
 * @return The value to be written to the module field of a routing mux.
 */
uint32_t AnalogTriggerOutput::GetModuleForRouting()
{
	return 0;
}

/**
 * @return The value to be written to the module field of a routing mux.
 */
bool AnalogTriggerOutput::GetAnalogTriggerForRouting()
{
	return true;
}

/**
 * Request interrupts asynchronously on this analog trigger output.
 * @param handler The address of the interrupt handler function of type tInterruptHandler that
 * will be called whenever there is an interrupt on the digitial input port.
 * Request interrupts in synchronus mode where the user program interrupt handler will be
 * called when an interrupt occurs.
 */
void AnalogTriggerOutput::RequestInterrupts(InterruptHandlerFunction handler, void *param)
{
	if (StatusIsFatal()) return;
	uint32_t index = m_interrupts->Allocate("Async Interrupt");
	if (index == ~0ul)
	{
		CloneError(m_interrupts);
		return;
	}
	m_interruptIndex = index;

	AllocateInterrupts(false);

	int32_t status = 0;
	requestInterrupts(m_interrupt, GetModuleForRouting(), GetChannelForRouting(),
	                  GetAnalogTriggerForRouting(), &status);
	attachInterruptHandler(m_interrupt, handler, param, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Request interrupts synchronously on this analog trigger output.
 * Request interrupts in asynchronus mode where the user program will have to
 * explicitly wait for the interrupt to occur.
 */
void AnalogTriggerOutput::RequestInterrupts()
{
	if (StatusIsFatal()) return;
	uint32_t index = m_interrupts->Allocate("Sync Interrupt");
	if (index == ~0ul)
	{
		CloneError(m_interrupts);
		return;
	}
	m_interruptIndex = index;

	AllocateInterrupts(true);

	int32_t status = 0;
	requestInterrupts(m_interrupt, GetModuleForRouting(), GetChannelForRouting(),
					  GetAnalogTriggerForRouting(), &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}
