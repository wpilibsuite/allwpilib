// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogAccelerometer.h"

#include <hal/FRCUsageReporting.h>

#include "frc/Base.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

AnalogAccelerometer::AnalogAccelerometer(int channel)
    : AnalogAccelerometer(std::make_shared<AnalogInput>(channel)) {
  SendableRegistry::GetInstance().AddChild(this, m_analogInput.get());
}

AnalogAccelerometer::AnalogAccelerometer(AnalogInput* channel)
    : m_analogInput(channel, NullDeleter<AnalogInput>()) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitAccelerometer();
  }
}

AnalogAccelerometer::AnalogAccelerometer(std::shared_ptr<AnalogInput> channel)
    : m_analogInput(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitAccelerometer();
  }
}

double AnalogAccelerometer::GetAcceleration() const {
  return (m_analogInput->GetAverageVoltage() - m_zeroGVoltage) / m_voltsPerG;
}

void AnalogAccelerometer::SetSensitivity(double sensitivity) {
  m_voltsPerG = sensitivity;
}

void AnalogAccelerometer::SetZero(double zero) {
  m_zeroGVoltage = zero;
}

double AnalogAccelerometer::PIDGet() {
  return GetAcceleration();
}

void AnalogAccelerometer::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Accelerometer");
  builder.AddDoubleProperty(
      "Value", [=]() { return GetAcceleration(); }, nullptr);
}

void AnalogAccelerometer::InitAccelerometer() {
  HAL_Report(HALUsageReporting::kResourceType_Accelerometer,
             m_analogInput->GetChannel() + 1);

  SendableRegistry::GetInstance().AddLW(this, "Accelerometer",
                                        m_analogInput->GetChannel());
}
