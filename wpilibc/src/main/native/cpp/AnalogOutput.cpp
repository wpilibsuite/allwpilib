/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogOutput.h"

#include <limits>
#include <utility>

#include <hal/AnalogOutput.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

AnalogOutput::AnalogOutput(int channel) {
  if (!SensorUtil::CheckAnalogOutputChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "analog output " + wpi::Twine(channel));
    m_channel = std::numeric_limits<int>::max();
    m_port = HAL_kInvalidHandle;
    return;
  }

  m_channel = channel;

  HAL_PortHandle port = HAL_GetPort(m_channel);
  int32_t status = 0;
  m_port = HAL_InitializeAnalogOutputPort(port, &status);
  if (status != 0) {
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumAnalogOutputs(), channel);
    m_channel = std::numeric_limits<int>::max();
    m_port = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_AnalogOutput, m_channel + 1);
  SendableRegistry::GetInstance().AddLW(this, "AnalogOutput", m_channel);
}

AnalogOutput::~AnalogOutput() { HAL_FreeAnalogOutputPort(m_port); }

void AnalogOutput::SetVoltage(double voltage) {
  int32_t status = 0;
  HAL_SetAnalogOutput(m_port, voltage, &status);

  wpi_setHALError(status);
}

double AnalogOutput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogOutput(m_port, &status);

  wpi_setHALError(status);

  return voltage;
}

int AnalogOutput::GetChannel() const { return m_channel; }

void AnalogOutput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Output");
  builder.AddDoubleProperty(
      "Value", [=]() { return GetVoltage(); },
      [=](double value) { SetVoltage(value); });
}
