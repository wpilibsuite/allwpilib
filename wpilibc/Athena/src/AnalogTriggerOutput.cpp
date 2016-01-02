/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogTriggerOutput.h"
#include "AnalogTrigger.h"
#include "WPIErrors.h"

/**
 * Create an object that represents one of the four outputs from an analog
 * trigger.
 *
 * Because this class derives from DigitalSource, it can be passed into routing
 * functions
 * for Counter, Encoder, etc.
 *
 * @param trigger A pointer to the trigger for which this is an output.
 * @param outputType An enum that specifies the output on the trigger to
 * represent.
 */
AnalogTriggerOutput::AnalogTriggerOutput(const AnalogTrigger &trigger,
                                         AnalogTriggerType outputType)
    : m_trigger(trigger), m_outputType(outputType) {
  HALReport(HALUsageReporting::kResourceType_AnalogTriggerOutput,
            trigger.GetIndex(), outputType);
}

AnalogTriggerOutput::~AnalogTriggerOutput() {
  if (m_interrupt != nullptr) {
    int32_t status = 0;
    cleanInterrupts(m_interrupt, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    m_interrupt = nullptr;
    m_interrupts->Free(m_interruptIndex);
  }
}

/**
 * Get the state of the analog trigger output.
 * @return The state of the analog trigger output.
 */
bool AnalogTriggerOutput::Get() const {
  int32_t status = 0;
  bool result =
      getAnalogTriggerOutput(m_trigger.m_trigger, m_outputType, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return result;
}

/**
 * @return The value to be written to the channel field of a routing mux.
 */
uint32_t AnalogTriggerOutput::GetChannelForRouting() const {
  return (m_trigger.m_index << 2) + m_outputType;
}

/**
 * @return The value to be written to the module field of a routing mux.
 */
uint32_t AnalogTriggerOutput::GetModuleForRouting() const { return 0; }

/**
 * @return The value to be written to the module field of a routing mux.
 */
bool AnalogTriggerOutput::GetAnalogTriggerForRouting() const { return true; }
