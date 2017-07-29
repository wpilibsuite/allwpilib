/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogPotentiometer.h"

/**
 * Common initialization code called by all constructors.
 */
void frc::AnalogPotentiometer::initPot(AnalogInput* input, double scale,
                                       double offset) {
  m_scale = scale;
  m_offset = offset;
  m_analog_input = input;
}

frc::AnalogPotentiometer::AnalogPotentiometer(int channel, double scale,
                                              double offset) {
  m_init_analog_input = true;
  initPot(new AnalogInput(channel), scale, offset);
}

frc::AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double scale,
                                              double offset) {
  m_init_analog_input = false;
  initPot(input, scale, offset);
}

frc::AnalogPotentiometer::AnalogPotentiometer(AnalogInput& input, double scale,
                                              double offset) {
  m_init_analog_input = false;
  initPot(&input, scale, offset);
}

frc::AnalogPotentiometer::~AnalogPotentiometer() {
  if (m_init_analog_input) {
    delete m_analog_input;
    m_init_analog_input = false;
  }
}

/**
 * Get the current reading of the potentiomere.
 *
 * @return The current position of the potentiometer.
 */
double frc::AnalogPotentiometer::Get() const {
  return m_analog_input->GetVoltage() * m_scale + m_offset;
}

/**
 * Implement the PIDSource interface.
 *
 * @return The current reading.
 */
double frc::AnalogPotentiometer::PIDGet() { return Get(); }

/**
 * @return the Smart Dashboard Type
 */
std::string frc::AnalogPotentiometer::GetSmartDashboardType() const {
  return "Analog Input";
}

/**
 * Live Window code, only does anything if live window is activated.
 */
void frc::AnalogPotentiometer::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  UpdateTable();
}

void frc::AnalogPotentiometer::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", Get());
  }
}

std::shared_ptr<ITable> frc::AnalogPotentiometer::GetTable() const {
  return m_table;
}
