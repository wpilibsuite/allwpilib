/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInput.h"

#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

/**
 * Construct an analog input.
 *
 * @param channel The channel number to represent.
 */
frc::AnalogInput::AnalogInput(int channel) : m_channel(channel) {
  llvm::SmallString<32> buf;
  llvm::raw_svector_ostream oss(buf);
  oss << "analog/" << channel;
  m_impl = new SimFloatInput(oss.str());

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
double frc::AnalogInput::GetVoltage() const { return m_impl->Get(); }

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
double frc::AnalogInput::GetAverageVoltage() const { return m_impl->Get(); }

/**
 * Get the channel number.
 *
 * @return The channel number.
 */
int frc::AnalogInput::GetChannel() const { return m_channel; }

/**
 * Get the Average value for the PID Source base object.
 *
 * @return The average voltage.
 */
double frc::AnalogInput::PIDGet() { return GetAverageVoltage(); }

void frc::AnalogInput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetAverageVoltage());
  }
}

void frc::AnalogInput::StartLiveWindowMode() {}

void frc::AnalogInput::StopLiveWindowMode() {}

std::string frc::AnalogInput::GetSmartDashboardType() const {
  return "Analog Input";
}

void frc::AnalogInput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> frc::AnalogInput::GetTable() const { return m_table; }
