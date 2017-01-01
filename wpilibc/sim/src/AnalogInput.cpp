/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInput.h"

#include <sstream>

#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Construct an analog input.
 *
 * @param channel The channel number to represent.
 */
AnalogInput::AnalogInput(int channel) : m_channel(channel) {
  std::stringstream ss;
  ss << "analog/" << channel;
  m_impl = new SimFloatInput(ss.str());

  LiveWindow::GetInstance()->AddSensor("AnalogInput", channel, this);
}

/**
 * Get a scaled sample straight from this channel.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 *
 * @return A scaled sample straight from this channel.
 */
double AnalogInput::GetVoltage() const { return m_impl->Get(); }

/**
 * Get a scaled sample from the output of the oversample and average engine for
 * this channel.
 *
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset(). Using oversampling will cause this value to
 * be higher resolution, but it will update more slowly. Using averaging will
 * cause this value to be more stable, but it will update more slowly.
 *
 * @return A scaled sample from the output of the oversample and average engine
 *         for this channel.
 */
double AnalogInput::GetAverageVoltage() const { return m_impl->Get(); }

/**
 * Get the channel number.
 *
 * @return The channel number.
 */
int AnalogInput::GetChannel() const { return m_channel; }

/**
 * Get the Average value for the PID Source base object.
 *
 * @return The average voltage.
 */
double AnalogInput::PIDGet() { return GetAverageVoltage(); }

void AnalogInput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetAverageVoltage());
  }
}

void AnalogInput::StartLiveWindowMode() {}

void AnalogInput::StopLiveWindowMode() {}

std::string AnalogInput::GetSmartDashboardType() const {
  return "Analog Input";
}

void AnalogInput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> AnalogInput::GetTable() const { return m_table; }
