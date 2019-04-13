/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogOutput.h"

#include <limits>
#include <utility>

#include <hal/HAL.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

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
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumAnalogOutputs(), channel,
                                 HAL_GetErrorMessage(status));
    m_channel = std::numeric_limits<int>::max();
    m_port = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_AnalogOutput, m_channel);
  SetName("AnalogOutput", m_channel);
}

AnalogOutput::~AnalogOutput() { HAL_FreeAnalogOutputPort(m_port); }

AnalogOutput::AnalogOutput(AnalogOutput&& rhs)
    : ErrorBase(std::move(rhs)),
      SendableBase(std::move(rhs)),
      m_channel(std::move(rhs.m_channel)) {
  std::swap(m_port, rhs.m_port);
}

AnalogOutput& AnalogOutput::operator=(AnalogOutput&& rhs) {
  ErrorBase::operator=(std::move(rhs));
  SendableBase::operator=(std::move(rhs));

  m_channel = std::move(rhs.m_channel);
  std::swap(m_port, rhs.m_port);

  return *this;
}

void AnalogOutput::SetVoltage(double voltage) {
  int32_t status = 0;
  HAL_SetAnalogOutput(m_port, voltage, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

double AnalogOutput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogOutput(m_port, &status);

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  return voltage;
}

int AnalogOutput::GetChannel() { return m_channel; }

void AnalogOutput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Output");
  builder.AddDoubleProperty("Value", [=]() { return GetVoltage(); },
                            [=](double value) { SetVoltage(value); });
}
