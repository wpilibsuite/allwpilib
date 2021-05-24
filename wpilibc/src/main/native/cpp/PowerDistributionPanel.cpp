// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PowerDistributionPanel.h"

#include <hal/FRCUsageReporting.h>
#include <hal/PDP.h>
#include <hal/Ports.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PowerDistributionPanel::PowerDistributionPanel() : PowerDistributionPanel(0) {}

/**
 * Initialize the PDP.
 */
PowerDistributionPanel::PowerDistributionPanel(int module) : m_module(module) {
  int32_t status = 0;
  m_handle = HAL_InitializePDP(module, &status);
  FRC_CheckErrorStatus(status, "Module {}", module);

  HAL_Report(HALUsageReporting::kResourceType_PDP, module + 1);
  SendableRegistry::GetInstance().AddLW(this, "PowerDistributionPanel", module);
}

double PowerDistributionPanel::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetPDPVoltage(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return voltage;
}

double PowerDistributionPanel::GetTemperature() const {
  int32_t status = 0;
  double temperature = HAL_GetPDPTemperature(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return temperature;
}

double PowerDistributionPanel::GetCurrent(int channel) const {
  int32_t status = 0;

  if (!SensorUtil::CheckPDPChannel(channel)) {
    FRC_ReportError(err::ChannelIndexOutOfRange, "Module {} Channel {}",
                    m_module, channel);
    return 0;
  }

  double current = HAL_GetPDPChannelCurrent(m_handle, channel, &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_module, channel);

  return current;
}

double PowerDistributionPanel::GetTotalCurrent() const {
  int32_t status = 0;
  double current = HAL_GetPDPTotalCurrent(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return current;
}

double PowerDistributionPanel::GetTotalPower() const {
  int32_t status = 0;
  double power = HAL_GetPDPTotalPower(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return power;
}

double PowerDistributionPanel::GetTotalEnergy() const {
  int32_t status = 0;
  double energy = HAL_GetPDPTotalEnergy(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return energy;
}

void PowerDistributionPanel::ResetTotalEnergy() {
  int32_t status = 0;
  HAL_ResetPDPTotalEnergy(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

void PowerDistributionPanel::ClearStickyFaults() {
  int32_t status = 0;
  HAL_ClearPDPStickyFaults(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

int PowerDistributionPanel::GetModule() const {
  return m_module;
}

void PowerDistributionPanel::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PowerDistributionPanel");
  for (int i = 0; i < SensorUtil::kPDPChannels; ++i) {
    builder.AddDoubleProperty(
        "Chan" + wpi::Twine(i), [=]() { return GetCurrent(i); }, nullptr);
  }
  builder.AddDoubleProperty(
      "Voltage", [=]() { return GetVoltage(); }, nullptr);
  builder.AddDoubleProperty(
      "TotalCurrent", [=]() { return GetTotalCurrent(); }, nullptr);
}
