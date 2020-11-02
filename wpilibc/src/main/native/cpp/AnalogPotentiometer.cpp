/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogPotentiometer.h"

#include "frc/Base.h"
#include "frc/RobotController.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::make_shared<AnalogInput>(channel), fullRange,
                          offset) {
  SendableRegistry::GetInstance().AddChild(this, m_analog_input.get());
}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : AnalogPotentiometer(
          std::shared_ptr<AnalogInput>(input, NullDeleter<AnalogInput>()),
          fullRange, offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(input), m_fullRange(fullRange), m_offset(offset) {
  SendableRegistry::GetInstance().AddLW(this, "AnalogPotentiometer",
                                        m_analog_input->GetChannel());
}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetAverageVoltage() /
          RobotController::GetVoltage5V()) *
             m_fullRange +
         m_offset;
}

double AnalogPotentiometer::PIDGet() { return Get(); }

void AnalogPotentiometer::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty(
      "Value", [=]() { return Get(); }, nullptr);
}
