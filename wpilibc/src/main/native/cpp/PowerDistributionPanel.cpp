/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PowerDistributionPanel.h"

#include <hal/HAL.h>
#include <hal/PDP.h>
#include <hal/Ports.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

PowerDistributionPanel::PowerDistributionPanel() : PowerDistributionPanel(0) {}

/**
 * Initialize the PDP.
 */
PowerDistributionPanel::PowerDistributionPanel(int module) {
  int32_t status = 0;
  m_handle = HAL_InitializePDP(module, &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumPDPModules(), module,
                                 HAL_GetErrorMessage(status));
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_PDP, module);
  SetName("PowerDistributionPanel", module);
}

double PowerDistributionPanel::GetVoltage() const {
  int32_t status = 0;

  double voltage = HAL_GetPDPVoltage(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return voltage;
}

double PowerDistributionPanel::GetTemperature() const {
  int32_t status = 0;

  double temperature = HAL_GetPDPTemperature(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return temperature;
}

double PowerDistributionPanel::GetCurrent(int channel) const {
  int32_t status = 0;

  if (!SensorUtil::CheckPDPChannel(channel)) {
    wpi::SmallString<32> str;
    wpi::raw_svector_ostream buf(str);
    buf << "PDP Channel " << channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
  }

  double current = HAL_GetPDPChannelCurrent(m_handle, channel, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return current;
}

double PowerDistributionPanel::GetTotalCurrent() const {
  int32_t status = 0;

  double current = HAL_GetPDPTotalCurrent(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return current;
}

double PowerDistributionPanel::GetTotalPower() const {
  int32_t status = 0;

  double power = HAL_GetPDPTotalPower(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return power;
}

double PowerDistributionPanel::GetTotalEnergy() const {
  int32_t status = 0;

  double energy = HAL_GetPDPTotalEnergy(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }

  return energy;
}

void PowerDistributionPanel::ResetTotalEnergy() {
  int32_t status = 0;

  HAL_ResetPDPTotalEnergy(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }
}

void PowerDistributionPanel::ClearStickyFaults() {
  int32_t status = 0;

  HAL_ClearPDPStickyFaults(m_handle, &status);

  if (status) {
    wpi_setWPIErrorWithContext(Timeout, "");
  }
}

void PowerDistributionPanel::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PowerDistributionPanel");
  for (int i = 0; i < SensorUtil::kPDPChannels; ++i) {
    builder.AddDoubleProperty("Chan" + wpi::Twine(i),
                              [=]() { return GetCurrent(i); }, nullptr);
  }
  builder.AddDoubleProperty("Voltage", [=]() { return GetVoltage(); }, nullptr);
  builder.AddDoubleProperty("TotalCurrent", [=]() { return GetTotalCurrent(); },
                            nullptr);
}
