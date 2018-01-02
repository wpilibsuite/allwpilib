/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutput.h"

#include <limits>

#include <HAL/HAL.h>
#include <HAL/Ports.h>

#include "SensorBase.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Construct an analog output on the given channel.
 *
 * All analog outputs are located on the MXP port.
 *
 * @param channel The channel number on the roboRIO to represent.
 */
AnalogOutput::AnalogOutput(int channel) {
  if (!SensorBase::CheckAnalogOutputChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "analog output " + llvm::Twine(channel));
    m_channel = std::numeric_limits<int>::max();
    m_port = HAL_kInvalidHandle;
    return;
  }

  m_channel = channel;

  HAL_PortHandle port = HAL_GetPort(m_channel);
  int32_t status = 0;
  m_port = HAL_InitializeAnalogOutputPort(port, &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumAnalogOutputs(), channel,
                                 HAL_GetErrorMessage(status));
    m_channel = std::numeric_limits<int>::max();
    m_port = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_AnalogOutput, m_channel);
  SetName("AnalogOutput", m_channel);
}

/**
 * Destructor.
 *
 * Frees analog output resource.
 */
AnalogOutput::~AnalogOutput() { HAL_FreeAnalogOutputPort(m_port); }

/**
 * Get the channel of this AnalogOutput.
 */
int AnalogOutput::GetChannel() { return m_channel; }

/**
 * Set the value of the analog output.
 *
 * @param voltage The output value in Volts, from 0.0 to +5.0
 */
void AnalogOutput::SetVoltage(double voltage) {
  int32_t status = 0;
  HAL_SetAnalogOutput(m_port, voltage, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Get the voltage of the analog output
 *
 * @return The value in Volts, from 0.0 to +5.0
 */
double AnalogOutput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogOutput(m_port, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  return voltage;
}

void AnalogOutput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Output");
  builder.AddDoubleProperty("Value", [=]() { return GetVoltage(); },
                            [=](double value) { SetVoltage(value); });
}
