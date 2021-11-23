// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PowerDistribution.h"

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <hal/PowerDistribution.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

static_assert(static_cast<HAL_PowerDistributionType>(
                  frc::PowerDistribution::ModuleType::kCTRE) ==
              HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE);
static_assert(static_cast<HAL_PowerDistributionType>(
                  frc::PowerDistribution::ModuleType::kRev) ==
              HAL_PowerDistributionType::HAL_PowerDistributionType_kRev);
static_assert(frc::PowerDistribution::kDefaultModule ==
              HAL_DEFAULT_POWER_DISTRIBUTION_MODULE);

using namespace frc;

PowerDistribution::PowerDistribution() {
  auto stack = wpi::GetStackTrace(1);

  int32_t status = 0;
  m_handle = HAL_InitializePowerDistribution(
      kDefaultModule, HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic, stack.c_str(),
      &status);
  FRC_CheckErrorStatus(status, "Module {}", kDefaultModule);
  m_module = HAL_GetPowerDistributionModuleNumber(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);

  HAL_Report(HALUsageReporting::kResourceType_PDP, m_module + 1);
  wpi::SendableRegistry::AddLW(this, "PowerDistribution", m_module);
}

PowerDistribution::PowerDistribution(int module, ModuleType moduleType) {
  auto stack = wpi::GetStackTrace(1);

  int32_t status = 0;
  m_handle = HAL_InitializePowerDistribution(
      module, static_cast<HAL_PowerDistributionType>(moduleType), stack.c_str(),
      &status);
  FRC_CheckErrorStatus(status, "Module {}", module);
  m_module = HAL_GetPowerDistributionModuleNumber(m_handle, &status);
  FRC_ReportError(status, "Module {}", module);

  HAL_Report(HALUsageReporting::kResourceType_PDP, m_module + 1);
  wpi::SendableRegistry::AddLW(this, "PowerDistribution", m_module);
}

PowerDistribution::~PowerDistribution() {
  if (m_handle != HAL_kInvalidHandle) {
    HAL_CleanPowerDistribution(m_handle);
    m_handle = HAL_kInvalidHandle;
  }
}

double PowerDistribution::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetPowerDistributionVoltage(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return voltage;
}

double PowerDistribution::GetTemperature() const {
  int32_t status = 0;
  double temperature = HAL_GetPowerDistributionTemperature(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return temperature;
}

double PowerDistribution::GetCurrent(int channel) const {
  int32_t status = 0;
  double current =
      HAL_GetPowerDistributionChannelCurrent(m_handle, channel, &status);
  FRC_ReportError(status, "Module {} Channel {}", m_module, channel);

  return current;
}

double PowerDistribution::GetTotalCurrent() const {
  int32_t status = 0;
  double current = HAL_GetPowerDistributionTotalCurrent(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return current;
}

double PowerDistribution::GetTotalPower() const {
  int32_t status = 0;
  double power = HAL_GetPowerDistributionTotalPower(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return power;
}

double PowerDistribution::GetTotalEnergy() const {
  int32_t status = 0;
  double energy = HAL_GetPowerDistributionTotalEnergy(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return energy;
}

void PowerDistribution::ResetTotalEnergy() {
  int32_t status = 0;
  HAL_ResetPowerDistributionTotalEnergy(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PowerDistribution::ClearStickyFaults() {
  int32_t status = 0;
  HAL_ClearPowerDistributionStickyFaults(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

int PowerDistribution::GetModule() const {
  return m_module;
}

bool PowerDistribution::GetSwitchableChannel() const {
  int32_t status = 0;
  bool state = HAL_GetPowerDistributionSwitchableChannel(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return state;
}

void PowerDistribution::SetSwitchableChannel(bool enabled) {
  int32_t status = 0;
  HAL_SetPowerDistributionSwitchableChannel(m_handle, enabled, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PowerDistribution::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PowerDistribution");
  int32_t status = 0;
  int numChannels = HAL_GetPowerDistributionNumChannels(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  // Use manual reads to avoid printing errors
  for (int i = 0; i < numChannels; ++i) {
    builder.AddDoubleProperty(
        fmt::format("Chan{}", i),
        [=] {
          int32_t lStatus = 0;
          return HAL_GetPowerDistributionChannelCurrent(m_handle, i, &lStatus);
        },
        nullptr);
  }
  builder.AddDoubleProperty(
      "Voltage",
      [=] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionVoltage(m_handle, &lStatus);
      },
      nullptr);
  builder.AddDoubleProperty(
      "TotalCurrent",
      [=] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionTotalCurrent(m_handle, &lStatus);
      },
      nullptr);
  builder.AddBooleanProperty(
      "SwitchableChannel",
      [=] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionSwitchableChannel(m_handle, &lStatus);
      },
      [=](bool value) {
        int32_t lStatus = 0;
        HAL_SetPowerDistributionSwitchableChannel(m_handle, value, &lStatus);
      });
}
