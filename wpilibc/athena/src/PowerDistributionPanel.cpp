/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PowerDistributionPanel.h"

#include "HAL/HAL.h"
#include "HAL/PDP.h"
#include "HAL/Ports.h"
#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

using namespace frc;

PowerDistributionPanel::PowerDistributionPanel() : PowerDistributionPanel(0) {}

/**
 * Initialize the PDP.
 */
PowerDistributionPanel::PowerDistributionPanel(int module) : m_module(module) {
  int32_t status = 0;
  HAL_InitializePDP(m_module, &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumPDPModules(), module,
                                 HAL_GetErrorMessage(status));
    m_module = -1;
    return;
  }
}

/**
 * Query the input voltage of the PDP.
 *
 * @return The voltage of the PDP in volts
 */
double PowerDistributionPanel::GetVoltage() const {
  int32_t status = 0;

  double voltage = HAL_GetPDPVoltage(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return voltage;
}

/**
 * Query the temperature of the PDP.
 *
 * @return The temperature of the PDP in degrees Celsius
 */
double PowerDistributionPanel::GetTemperature() const {
  int32_t status = 0;

  double temperature = HAL_GetPDPTemperature(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return temperature;
}

/**
 * Query the current of a single channel of the PDP.
 *
 * @return The current of one of the PDP channels (channels 0-15) in Amperes
 */
double PowerDistributionPanel::GetCurrent(int channel) const {
  int32_t status = 0;

  if (!CheckPDPChannel(channel)) {
    llvm::SmallString<32> str;
    llvm::raw_svector_ostream buf(str);
    buf << "PDP Channel " << channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
  }

  double current = HAL_GetPDPChannelCurrent(m_module, channel, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return current;
}

/**
 * Query the total current of all monitored PDP channels (0-15).
 *
 * @return The the total current drawn from the PDP channels in Amperes
 */
double PowerDistributionPanel::GetTotalCurrent() const {
  int32_t status = 0;

  double current = HAL_GetPDPTotalCurrent(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return current;
}

/**
 * Query the total power drawn from the monitored PDP channels.
 *
 * @return The the total power drawn from the PDP channels in Watts
 */
double PowerDistributionPanel::GetTotalPower() const {
  int32_t status = 0;

  double power = HAL_GetPDPTotalPower(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return power;
}

/**
 * Query the total energy drawn from the monitored PDP channels.
 *
 * @return The the total energy drawn from the PDP channels in Joules
 */
double PowerDistributionPanel::GetTotalEnergy() const {
  int32_t status = 0;

  double energy = HAL_GetPDPTotalEnergy(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return energy;
}

/**
 * Reset the total energy drawn from the PDP.
 *
 * @see PowerDistributionPanel#GetTotalEnergy
 */
void PowerDistributionPanel::ResetTotalEnergy() {
  int32_t status = 0;

  HAL_ResetPDPTotalEnergy(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }
}

/**
 * Remove all of the fault flags on the PDP.
 */
void PowerDistributionPanel::ClearStickyFaults() {
  int32_t status = 0;

  HAL_ClearPDPStickyFaults(m_module, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }
}

void PowerDistributionPanel::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Chan0", GetCurrent(0));
    m_table->PutNumber("Chan1", GetCurrent(1));
    m_table->PutNumber("Chan2", GetCurrent(2));
    m_table->PutNumber("Chan3", GetCurrent(3));
    m_table->PutNumber("Chan4", GetCurrent(4));
    m_table->PutNumber("Chan5", GetCurrent(5));
    m_table->PutNumber("Chan6", GetCurrent(6));
    m_table->PutNumber("Chan7", GetCurrent(7));
    m_table->PutNumber("Chan8", GetCurrent(8));
    m_table->PutNumber("Chan9", GetCurrent(9));
    m_table->PutNumber("Chan10", GetCurrent(10));
    m_table->PutNumber("Chan11", GetCurrent(11));
    m_table->PutNumber("Chan12", GetCurrent(12));
    m_table->PutNumber("Chan13", GetCurrent(13));
    m_table->PutNumber("Chan14", GetCurrent(14));
    m_table->PutNumber("Chan15", GetCurrent(15));
    m_table->PutNumber("Voltage", GetVoltage());
    m_table->PutNumber("TotalCurrent", GetTotalCurrent());
  }
}

void PowerDistributionPanel::StartLiveWindowMode() {}

void PowerDistributionPanel::StopLiveWindowMode() {}

std::string PowerDistributionPanel::GetSmartDashboardType() const {
  return "PowerDistributionPanel";
}

void PowerDistributionPanel::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> PowerDistributionPanel::GetTable() const {
  return m_table;
}
