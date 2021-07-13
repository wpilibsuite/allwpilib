// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <hal/PowerDistribution.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/PowerDistribution.h"
#include "frc/SensorUtil.h"

using namespace frc;

PowerDistribution::PowerDistribution() : PowerDistribution(0) {}

PowerDistribution::PowerDistribution(int module) : m_module(module) {
  int32_t status = 0;
  m_handle = HAL_InitializePowerDistribution(
      module, HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic,
      &status);
  FRC_CheckErrorStatus(status, "Module {}", module);

  HAL_Report(HALUsageReporting::kResourceType_PDP, module + 1);
  wpi::SendableRegistry::AddLW(this, "PowerDistribution", module);
}

double PowerDistribution::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetPowerDistributionVoltage(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return voltage;
}

double PowerDistribution::GetTemperature() const {
  int32_t status = 0;
  double temperature = HAL_GetPowerDistributionTemperature(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return temperature;
}

double PowerDistribution::GetCurrent(int channel) const {
  int32_t status = 0;

  if (!HAL_CheckPowerDistributionChannel(m_handle, channel)) {
    FRC_ReportError(err::ChannelIndexOutOfRange, "Module {} Channel {}",
                    m_module, channel);
    return 0;
  }

  double current =
      HAL_GetPowerDistributionChannelCurrent(m_handle, channel, &status);
  FRC_CheckErrorStatus(status, "Module {} Channel {}", m_module, channel);

  return current;
}

double PowerDistribution::GetTotalCurrent() const {
  int32_t status = 0;
  double current = HAL_GetPowerDistributionTotalCurrent(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return current;
}

double PowerDistribution::GetTotalPower() const {
  int32_t status = 0;
  double power = HAL_GetPowerDistributionTotalPower(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return power;
}

double PowerDistribution::GetTotalEnergy() const {
  int32_t status = 0;
  double energy = HAL_GetPowerDistributionTotalEnergy(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return energy;
}

void PowerDistribution::ResetTotalEnergy() {
  int32_t status = 0;
  HAL_ResetPowerDistributionTotalEnergy(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

void PowerDistribution::ClearStickyFaults() {
  int32_t status = 0;
  HAL_ClearPowerDistributionStickyFaults(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

int PowerDistribution::GetModule() const {
  return m_module;
}

void PowerDistribution::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PowerDistribution");
  for (int i = 0; i < SensorUtil::kPDPChannels; ++i) {
    builder.AddDoubleProperty(
        fmt::format("Chan{}", i), [=] { return GetCurrent(i); }, nullptr);
  }
  builder.AddDoubleProperty(
      "Voltage", [=] { return GetVoltage(); }, nullptr);
  builder.AddDoubleProperty(
      "TotalCurrent", [=] { return GetTotalCurrent(); }, nullptr);
}
