/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogPotentiometer.h"

#include "frc/RobotController.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : m_analog_input(std::make_shared<AnalogInput>(channel)),
      m_fullRange(fullRange),
      m_offset(offset) {
  AddChild(m_analog_input);
}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : m_analog_input(input, NullDeleter<AnalogInput>()),
      m_fullRange(fullRange),
      m_offset(offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(input), m_fullRange(fullRange), m_offset(offset) {}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetVoltage() / RobotController::GetVoltage5V()) *
             m_fullRange +
         m_offset;
}

double AnalogPotentiometer::PIDGet() { return Get(); }

void AnalogPotentiometer::InitSendable(SendableBuilder& builder) {
  m_analog_input->InitSendable(builder);
}
