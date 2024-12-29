// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PowerDistribution.h"

#include <vector>

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <hal/PowerDistribution.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"

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
      kDefaultModule,
      HAL_PowerDistributionType::HAL_PowerDistributionType_kAutomatic,
      stack.c_str(), &status);
  FRC_CheckErrorStatus(status, "Module {}", kDefaultModule);
  m_module = HAL_GetPowerDistributionModuleNumber(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);

  if (HAL_GetPowerDistributionType(m_handle, &status) ==
      HAL_PowerDistributionType::HAL_PowerDistributionType_kCTRE) {
    HAL_Report(HALUsageReporting::kResourceType_PDP,
               HALUsageReporting::kPDP_CTRE);
  } else {
    HAL_Report(HALUsageReporting::kResourceType_PDP,
               HALUsageReporting::kPDP_REV);
  }
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

  if (moduleType == ModuleType::kCTRE) {
    HAL_Report(HALUsageReporting::kResourceType_PDP,
               HALUsageReporting::kPDP_CTRE);
  } else {
    HAL_Report(HALUsageReporting::kResourceType_PDP,
               HALUsageReporting::kPDP_REV);
  }
  wpi::SendableRegistry::AddLW(this, "PowerDistribution", m_module);
}

int PowerDistribution::GetNumChannels() const {
  int32_t status = 0;
  int32_t size = HAL_GetPowerDistributionNumChannels(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return size;
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

std::vector<double> PowerDistribution::GetAllCurrents() const {
  int32_t status = 0;
  int32_t size = GetNumChannels();
  std::vector<double> currents(size);

  HAL_GetPowerDistributionAllChannelCurrents(m_handle, currents.data(), size,
                                             &status);
  FRC_ReportError(status, "Module {}", m_module);
  return currents;
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

PowerDistribution::ModuleType PowerDistribution::GetType() const {
  int32_t status = 0;
  auto type = HAL_GetPowerDistributionType(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return static_cast<ModuleType>(type);
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

PowerDistribution::Version PowerDistribution::GetVersion() const {
  int32_t status = 0;
  HAL_PowerDistributionVersion halVersion;
  std::memset(&halVersion, 0, sizeof(halVersion));
  HAL_GetPowerDistributionVersion(m_handle, &halVersion, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PowerDistribution::Version version;
  static_assert(sizeof(halVersion) == sizeof(version));
  static_assert(std::is_standard_layout_v<decltype(version)>);
  static_assert(std::is_trivial_v<decltype(version)>);
  std::memcpy(&version, &halVersion, sizeof(version));
  return version;
}

PowerDistribution::Faults PowerDistribution::GetFaults() const {
  int32_t status = 0;
  HAL_PowerDistributionFaults halFaults;
  std::memset(&halFaults, 0, sizeof(halFaults));
  HAL_GetPowerDistributionFaults(m_handle, &halFaults, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PowerDistribution::Faults faults;
  static_assert(sizeof(halFaults) == sizeof(faults));
  static_assert(std::is_standard_layout_v<decltype(faults)>);
  static_assert(std::is_trivial_v<decltype(faults)>);
  std::memcpy(&faults, &halFaults, sizeof(faults));
  return faults;
}

bool PowerDistribution::Faults::GetBreakerFault(int channel) const {
  switch (channel) {
    case 0:
      return Channel0BreakerFault != 0;
    case 1:
      return Channel1BreakerFault != 0;
    case 2:
      return Channel2BreakerFault != 0;
    case 3:
      return Channel3BreakerFault != 0;
    case 4:
      return Channel4BreakerFault != 0;
    case 5:
      return Channel5BreakerFault != 0;
    case 6:
      return Channel6BreakerFault != 0;
    case 7:
      return Channel7BreakerFault != 0;
    case 8:
      return Channel8BreakerFault != 0;
    case 9:
      return Channel9BreakerFault != 0;
    case 10:
      return Channel10BreakerFault != 0;
    case 11:
      return Channel11BreakerFault != 0;
    case 12:
      return Channel12BreakerFault != 0;
    case 13:
      return Channel13BreakerFault != 0;
    case 14:
      return Channel14BreakerFault != 0;
    case 15:
      return Channel15BreakerFault != 0;
    case 16:
      return Channel16BreakerFault != 0;
    case 17:
      return Channel17BreakerFault != 0;
    case 18:
      return Channel18BreakerFault != 0;
    case 19:
      return Channel19BreakerFault != 0;
    case 20:
      return Channel20BreakerFault != 0;
    case 21:
      return Channel21BreakerFault != 0;
    case 22:
      return Channel22BreakerFault != 0;
    case 23:
      return Channel23BreakerFault != 0;
    default:
      throw FRC_MakeError(err::ChannelIndexOutOfRange,
                          "Power distribution fault channel out of bounds!");
  }
}

bool PowerDistribution::StickyFaults::GetBreakerFault(int channel) const {
  switch (channel) {
    case 0:
      return Channel0BreakerFault != 0;
    case 1:
      return Channel1BreakerFault != 0;
    case 2:
      return Channel2BreakerFault != 0;
    case 3:
      return Channel3BreakerFault != 0;
    case 4:
      return Channel4BreakerFault != 0;
    case 5:
      return Channel5BreakerFault != 0;
    case 6:
      return Channel6BreakerFault != 0;
    case 7:
      return Channel7BreakerFault != 0;
    case 8:
      return Channel8BreakerFault != 0;
    case 9:
      return Channel9BreakerFault != 0;
    case 10:
      return Channel10BreakerFault != 0;
    case 11:
      return Channel11BreakerFault != 0;
    case 12:
      return Channel12BreakerFault != 0;
    case 13:
      return Channel13BreakerFault != 0;
    case 14:
      return Channel14BreakerFault != 0;
    case 15:
      return Channel15BreakerFault != 0;
    case 16:
      return Channel16BreakerFault != 0;
    case 17:
      return Channel17BreakerFault != 0;
    case 18:
      return Channel18BreakerFault != 0;
    case 19:
      return Channel19BreakerFault != 0;
    case 20:
      return Channel20BreakerFault != 0;
    case 21:
      return Channel21BreakerFault != 0;
    case 22:
      return Channel22BreakerFault != 0;
    case 23:
      return Channel23BreakerFault != 0;
    default:
      throw FRC_MakeError(err::ChannelIndexOutOfRange,
                          "Power distribution fault channel out of bounds!");
  }
}

PowerDistribution::StickyFaults PowerDistribution::GetStickyFaults() const {
  int32_t status = 0;
  HAL_PowerDistributionStickyFaults halStickyFaults;
  std::memset(&halStickyFaults, 0, sizeof(halStickyFaults));
  HAL_GetPowerDistributionStickyFaults(m_handle, &halStickyFaults, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PowerDistribution::StickyFaults stickyFaults;
  static_assert(sizeof(halStickyFaults) == sizeof(stickyFaults));
  static_assert(std::is_standard_layout_v<decltype(stickyFaults)>);
  static_assert(std::is_trivial_v<decltype(stickyFaults)>);
  std::memcpy(&stickyFaults, &halStickyFaults, sizeof(stickyFaults));
  return stickyFaults;
}

void PowerDistribution::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PowerDistribution");
  int numChannels = GetNumChannels();
  // Use manual reads to avoid printing errors
  for (int i = 0; i < numChannels; ++i) {
    builder.AddDoubleProperty(
        fmt::format("Chan{}", i),
        [=, this] {
          int32_t lStatus = 0;
          return HAL_GetPowerDistributionChannelCurrent(m_handle, i, &lStatus);
        },
        nullptr);
  }
  builder.AddDoubleProperty(
      "Voltage",
      [=, this] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionVoltage(m_handle, &lStatus);
      },
      nullptr);
  builder.AddDoubleProperty(
      "TotalCurrent",
      [=, this] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionTotalCurrent(m_handle, &lStatus);
      },
      nullptr);
  builder.AddBooleanProperty(
      "SwitchableChannel",
      [=, this] {
        int32_t lStatus = 0;
        return HAL_GetPowerDistributionSwitchableChannel(m_handle, &lStatus);
      },
      [=, this](bool value) {
        int32_t lStatus = 0;
        HAL_SetPowerDistributionSwitchableChannel(m_handle, value, &lStatus);
      });
}
