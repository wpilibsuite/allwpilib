// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticHub.h"

#include <array>
#include <cstdio>
#include <memory>
#include <string>

#include <fmt/format.h>
#include <hal/REVPH.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>

#include "frc/Compressor.h"
#include "frc/DoubleSolenoid.h"
#include "frc/Errors.h"
#include "frc/RobotBase.h"
#include "frc/SensorUtil.h"
#include "frc/Solenoid.h"

using namespace frc;

/** Converts volts to PSI per the REV Analog Pressure Sensor datasheet. */
units::pounds_per_square_inch_t VoltsToPSI(units::volt_t sensorVoltage,
                                           units::volt_t supplyVoltage) {
  return units::pounds_per_square_inch_t{
      250 * (sensorVoltage.value() / supplyVoltage.value()) - 25};
}

/** Converts PSI to volts per the REV Analog Pressure Sensor datasheet. */
units::volt_t PSIToVolts(units::pounds_per_square_inch_t pressure,
                         units::volt_t supplyVoltage) {
  return units::volt_t{supplyVoltage.value() *
                       (0.004 * pressure.value() + 0.1)};
}

wpi::mutex PneumaticHub::m_handleLock;
std::unique_ptr<wpi::DenseMap<int, std::weak_ptr<PneumaticHub::DataStore>>>
    PneumaticHub::m_handleMap = nullptr;

// Always called under lock, so we can avoid the double lock from the magic
// static
std::weak_ptr<PneumaticHub::DataStore>& PneumaticHub::GetDataStore(int module) {
  if (!m_handleMap) {
    m_handleMap = std::make_unique<
        wpi::DenseMap<int, std::weak_ptr<PneumaticHub::DataStore>>>();
  }
  return (*m_handleMap)[module];
}

class PneumaticHub::DataStore {
 public:
  explicit DataStore(int module, const char* stackTrace) {
    int32_t status = 0;
    HAL_REVPHHandle handle = HAL_InitializeREVPH(module, stackTrace, &status);
    FRC_CheckErrorStatus(status, "Module {}", module);
    m_moduleObject = PneumaticHub{handle, module};
    m_moduleObject.m_dataStore =
        std::shared_ptr<DataStore>{this, wpi::NullDeleter<DataStore>()};

    auto version = m_moduleObject.GetVersion();

    if (version.FirmwareMajor > 0 && RobotBase::IsReal()) {
      // Write PH firmware version to roboRIO
      std::FILE* file = nullptr;
      file = std::fopen(
          fmt::format("/tmp/frc_versions/REV_PH_{:0>2}_WPILib_Version.ini",
                      module)
              .c_str(),
          "w");
      if (file != nullptr) {
        std::fputs("[Version]\n", file);
        std::fputs(fmt::format("model=REV PH\n").c_str(), file);
        std::fputs(fmt::format("deviceID={:x}\n", (0x9052600 | module)).c_str(),
                   file);
        std::fputs(fmt::format("currentVersion={}.{}.{}", version.FirmwareMajor,
                               version.FirmwareMinor, version.FirmwareFix)
                       .c_str(),
                   file);
        std::fclose(file);
      }
    }

    // Check PH firmware version
    if (version.FirmwareMajor > 0 && version.FirmwareMajor < 22) {
      throw FRC_MakeError(
          err::AssertionFailure,
          "The Pneumatic Hub has firmware version {}.{}.{}, and must be "
          "updated to version 2022.0.0 or later using the REV Hardware Client",
          version.FirmwareMajor, version.FirmwareMinor, version.FirmwareFix);
    }
  }

  ~DataStore() noexcept { HAL_FreeREVPH(m_moduleObject.m_handle); }

  DataStore(DataStore&&) = delete;
  DataStore& operator=(DataStore&&) = delete;

 private:
  friend class PneumaticHub;
  uint32_t m_reservedMask{0};
  bool m_compressorReserved{false};
  wpi::mutex m_reservedLock;
  PneumaticHub m_moduleObject{HAL_kInvalidHandle, 0};
  std::array<units::millisecond_t, 16> m_oneShotDurMs{0_ms};
};

PneumaticHub::PneumaticHub()
    : PneumaticHub{SensorUtil::GetDefaultREVPHModule()} {}

PneumaticHub::PneumaticHub(int module) {
  std::string stackTrace = wpi::GetStackTrace(1);
  std::scoped_lock lock(m_handleLock);
  auto& res = GetDataStore(module);
  m_dataStore = res.lock();
  if (!m_dataStore) {
    m_dataStore = std::make_shared<DataStore>(module, stackTrace.c_str());
    res = m_dataStore;
  }
  m_handle = m_dataStore->m_moduleObject.m_handle;
  m_module = module;
}

PneumaticHub::PneumaticHub(HAL_REVPHHandle handle, int module)
    : m_handle{handle}, m_module{module} {}

bool PneumaticHub::GetCompressor() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressor(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

void PneumaticHub::DisableCompressor() {
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlDisabled(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticHub::EnableCompressorDigital() {
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlDigital(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticHub::EnableCompressorAnalog(
    units::pounds_per_square_inch_t minPressure,
    units::pounds_per_square_inch_t maxPressure) {
  if (minPressure >= maxPressure) {
    throw FRC_MakeError(err::InvalidParameter,
                        "maxPressure must be greater than minPressure");
  }
  if (minPressure < 0_psi || minPressure > 120_psi) {
    throw FRC_MakeError(err::ParameterOutOfRange,
                        "minPressure must be between 0 and 120 PSI, got {}",
                        minPressure);
  }
  if (maxPressure < 0_psi || maxPressure > 120_psi) {
    throw FRC_MakeError(err::ParameterOutOfRange,
                        "maxPressure must be between 0 and 120 PSI, got {}",
                        maxPressure);
  }

  // Send the voltage as it would be if the 5V rail was at exactly 5V.
  // The firmware will compensate for the real 5V rail voltage, which
  // can fluctuate somewhat over time.
  units::volt_t minAnalogVoltage = PSIToVolts(minPressure, 5_V);
  units::volt_t maxAnalogVoltage = PSIToVolts(maxPressure, 5_V);

  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlAnalog(m_handle, minAnalogVoltage.value(),
                                      maxAnalogVoltage.value(), &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticHub::EnableCompressorHybrid(
    units::pounds_per_square_inch_t minPressure,
    units::pounds_per_square_inch_t maxPressure) {
  if (minPressure >= maxPressure) {
    throw FRC_MakeError(err::InvalidParameter,
                        "maxPressure must be greater than minPressure");
  }
  if (minPressure < 0_psi || minPressure > 120_psi) {
    throw FRC_MakeError(err::ParameterOutOfRange,
                        "minPressure must be between 0 and 120 PSI, got {}",
                        minPressure);
  }
  if (maxPressure < 0_psi || maxPressure > 120_psi) {
    throw FRC_MakeError(err::ParameterOutOfRange,
                        "maxPressure must be between 0 and 120 PSI, got {}",
                        maxPressure);
  }

  // Send the voltage as it would be if the 5V rail was at exactly 5V.
  // The firmware will compensate for the real 5V rail voltage, which
  // can fluctuate somewhat over time.
  units::volt_t minAnalogVoltage = PSIToVolts(minPressure, 5_V);
  units::volt_t maxAnalogVoltage = PSIToVolts(maxPressure, 5_V);

  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlHybrid(m_handle, minAnalogVoltage.value(),
                                      maxAnalogVoltage.value(), &status);
  FRC_ReportError(status, "Module {}", m_module);
}

CompressorConfigType PneumaticHub::GetCompressorConfigType() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressorConfig(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return static_cast<CompressorConfigType>(result);
}

bool PneumaticHub::GetPressureSwitch() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHPressureSwitch(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

units::ampere_t PneumaticHub::GetCompressorCurrent() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressorCurrent(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::ampere_t{result};
}

void PneumaticHub::SetSolenoids(int mask, int values) {
  int32_t status = 0;
  HAL_SetREVPHSolenoids(m_handle, mask, values, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

int PneumaticHub::GetSolenoids() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoids(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

int PneumaticHub::GetModuleNumber() const {
  return m_module;
}

int PneumaticHub::GetSolenoidDisabledList() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoidDisabledList(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

void PneumaticHub::FireOneShot(int index) {
  int32_t status = 0;
  HAL_FireREVPHOneShot(m_handle, index,
                       m_dataStore->m_oneShotDurMs[index].value(), &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticHub::SetOneShotDuration(int index, units::second_t duration) {
  m_dataStore->m_oneShotDurMs[index] = duration;
}

bool PneumaticHub::CheckSolenoidChannel(int channel) const {
  return HAL_CheckREVPHSolenoidChannel(channel);
}

int PneumaticHub::CheckAndReserveSolenoids(int mask) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  uint32_t uMask = static_cast<uint32_t>(mask);
  if ((m_dataStore->m_reservedMask & uMask) != 0) {
    return m_dataStore->m_reservedMask & uMask;
  }
  m_dataStore->m_reservedMask |= uMask;
  return 0;
}

void PneumaticHub::UnreserveSolenoids(int mask) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_reservedMask &= ~(static_cast<uint32_t>(mask));
}

bool PneumaticHub::ReserveCompressor() {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  if (m_dataStore->m_compressorReserved) {
    return false;
  }
  m_dataStore->m_compressorReserved = true;
  return true;
}

void PneumaticHub::UnreserveCompressor() {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_compressorReserved = false;
}

PneumaticHub::Version PneumaticHub::GetVersion() const {
  int32_t status = 0;
  HAL_REVPHVersion halVersions;
  std::memset(&halVersions, 0, sizeof(halVersions));
  HAL_GetREVPHVersion(m_handle, &halVersions, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PneumaticHub::Version versions;
  static_assert(sizeof(halVersions) == sizeof(versions));
  static_assert(std::is_standard_layout_v<decltype(versions)>);
  static_assert(std::is_trivial_v<decltype(versions)>);
  std::memcpy(&versions, &halVersions, sizeof(versions));
  return versions;
}

PneumaticHub::Faults PneumaticHub::GetFaults() const {
  int32_t status = 0;
  HAL_REVPHFaults halFaults;
  std::memset(&halFaults, 0, sizeof(halFaults));
  HAL_GetREVPHFaults(m_handle, &halFaults, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PneumaticHub::Faults faults;
  static_assert(sizeof(halFaults) == sizeof(faults));
  static_assert(std::is_standard_layout_v<decltype(faults)>);
  static_assert(std::is_trivial_v<decltype(faults)>);
  std::memcpy(&faults, &halFaults, sizeof(faults));
  return faults;
}

PneumaticHub::StickyFaults PneumaticHub::GetStickyFaults() const {
  int32_t status = 0;
  HAL_REVPHStickyFaults halStickyFaults;
  std::memset(&halStickyFaults, 0, sizeof(halStickyFaults));
  HAL_GetREVPHStickyFaults(m_handle, &halStickyFaults, &status);
  FRC_ReportError(status, "Module {}", m_module);
  PneumaticHub::StickyFaults stickyFaults;
  static_assert(sizeof(halStickyFaults) == sizeof(stickyFaults));
  static_assert(std::is_standard_layout_v<decltype(stickyFaults)>);
  static_assert(std::is_trivial_v<decltype(stickyFaults)>);
  std::memcpy(&stickyFaults, &halStickyFaults, sizeof(stickyFaults));
  return stickyFaults;
}

bool PneumaticHub::Faults::GetChannelFault(int channel) const {
  switch (channel) {
    case 0:
      return Channel0Fault != 0;
    case 1:
      return Channel1Fault != 0;
    case 2:
      return Channel2Fault != 0;
    case 3:
      return Channel3Fault != 0;
    case 4:
      return Channel4Fault != 0;
    case 5:
      return Channel5Fault != 0;
    case 6:
      return Channel6Fault != 0;
    case 7:
      return Channel7Fault != 0;
    case 8:
      return Channel8Fault != 0;
    case 9:
      return Channel9Fault != 0;
    case 10:
      return Channel10Fault != 0;
    case 11:
      return Channel11Fault != 0;
    case 12:
      return Channel12Fault != 0;
    case 13:
      return Channel13Fault != 0;
    case 14:
      return Channel14Fault != 0;
    case 15:
      return Channel15Fault != 0;
    default:
      throw FRC_MakeError(err::ChannelIndexOutOfRange,
                          "Pneumatics fault channel out of bounds!");
  }
}

void PneumaticHub::ClearStickyFaults() {
  int32_t status = 0;
  HAL_ClearREVPHStickyFaults(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

units::volt_t PneumaticHub::GetInputVoltage() const {
  int32_t status = 0;
  auto voltage = HAL_GetREVPHVoltage(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::volt_t{voltage};
}

units::volt_t PneumaticHub::Get5VRegulatedVoltage() const {
  int32_t status = 0;
  auto voltage = HAL_GetREVPH5VVoltage(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::volt_t{voltage};
}

units::ampere_t PneumaticHub::GetSolenoidsTotalCurrent() const {
  int32_t status = 0;
  auto current = HAL_GetREVPHSolenoidCurrent(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::ampere_t{current};
}

units::volt_t PneumaticHub::GetSolenoidsVoltage() const {
  int32_t status = 0;
  auto voltage = HAL_GetREVPHSolenoidVoltage(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::volt_t{voltage};
}

units::volt_t PneumaticHub::GetAnalogVoltage(int channel) const {
  int32_t status = 0;
  auto voltage = HAL_GetREVPHAnalogVoltage(m_handle, channel, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::volt_t{voltage};
}

units::pounds_per_square_inch_t PneumaticHub::GetPressure(int channel) const {
  int32_t status = 0;
  auto sensorVoltage = HAL_GetREVPHAnalogVoltage(m_handle, channel, &status);
  FRC_ReportError(status, "Module {}", m_module);
  auto supplyVoltage = HAL_GetREVPH5VVoltage(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return VoltsToPSI(units::volt_t{sensorVoltage}, units::volt_t{supplyVoltage});
}

Solenoid PneumaticHub::MakeSolenoid(int channel) {
  return Solenoid{m_module, PneumaticsModuleType::REVPH, channel};
}

DoubleSolenoid PneumaticHub::MakeDoubleSolenoid(int forwardChannel,
                                                int reverseChannel) {
  return DoubleSolenoid{m_module, PneumaticsModuleType::REVPH, forwardChannel,
                        reverseChannel};
}

Compressor PneumaticHub::MakeCompressor() {
  return Compressor{m_module, PneumaticsModuleType::REVPH};
}

std::shared_ptr<PneumaticsBase> PneumaticHub::GetForModule(int module) {
  std::string stackTrace = wpi::GetStackTrace(1);
  std::scoped_lock lock(m_handleLock);
  auto& res = GetDataStore(module);
  std::shared_ptr<DataStore> dataStore = res.lock();
  if (!dataStore) {
    dataStore = std::make_shared<DataStore>(module, stackTrace.c_str());
    res = dataStore;
  }

  return std::shared_ptr<PneumaticsBase>{dataStore, &dataStore->m_moduleObject};
}
