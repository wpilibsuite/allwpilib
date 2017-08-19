/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogTriggerOutput.h"

#include "AnalogTrigger.h"
#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Create an object that represents one of the four outputs from an analog
 * trigger.
 *
 * Because this class derives from DigitalSource, it can be passed into routing
 * functions for Counter, Encoder, etc.
 *
 * @param trigger    A pointer to the trigger for which this is an output.
 * @param outputType An enum that specifies the output on the trigger to
 *                   represent.
 */
AnalogTriggerOutput::AnalogTriggerOutput(const AnalogTrigger& trigger,
                                         AnalogTriggerType outputType)
    : m_trigger(trigger), m_outputType(outputType) {
  HAL_Report(HALUsageReporting::kResourceType_AnalogTriggerOutput,
             trigger.GetIndex(), static_cast<uint8_t>(outputType));
}

AnalogTriggerOutput::~AnalogTriggerOutput() {
  if (m_interrupt != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_CleanInterrupts(m_interrupt, &status);
    // ignore status, as an invalid handle just needs to be ignored.
    m_interrupt = HAL_kInvalidHandle;
  }
}

/**
 * Get the state of the analog trigger output.
 *
 * @return The state of the analog trigger output.
 */
bool AnalogTriggerOutput::Get() const {
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerOutput(
      m_trigger.m_trigger, static_cast<HAL_AnalogTriggerType>(m_outputType),
      &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return result;
}

/**
 * @return The HAL Handle to the specified source.
 */
HAL_Handle AnalogTriggerOutput::GetPortHandleForRouting() const {
  return m_trigger.m_trigger;
}

/**
 * Is source an AnalogTrigger
 */
bool AnalogTriggerOutput::IsAnalogTrigger() const { return true; }

/**
 * @return The type of analog trigger output to be used.
 */
AnalogTriggerType AnalogTriggerOutput::GetAnalogTriggerTypeForRouting() const {
  return m_outputType;
}

/**
 * @return The channel of the source.
 */
int AnalogTriggerOutput::GetChannel() const { return m_trigger.m_index; }
