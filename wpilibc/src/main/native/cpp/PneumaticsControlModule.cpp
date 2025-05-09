// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticsControlModule.h"

#include <memory>
#include <string>

#include <fmt/format.h>
#include <hal/CTREPCM.h>
#include <hal/Ports.h>
#include <hal/UsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>

#include "frc/Compressor.h"
#include "frc/DoubleSolenoid.h"
#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/Solenoid.h"

using namespace frc;

wpi::mutex PneumaticsControlModule::m_handleLock;
std::unique_ptr<
    wpi::DenseMap<int, std::weak_ptr<PneumaticsControlModule::DataStore>>[]>
    PneumaticsControlModule::m_handleMaps = nullptr;

// Always called under lock, so we can avoid the double lock from the magic
// static
std::weak_ptr<PneumaticsControlModule::DataStore>&
PneumaticsControlModule::GetDataStore(int busId, int module) {
  int32_t numBuses = HAL_GetNumCanBuses();
  FRC_AssertMessage(busId >= 0 && busId < numBuses,
                    "Bus {} out of range. Must be [0-{}).", busId, numBuses);
  if (!m_handleMaps) {
    m_handleMaps = std::make_unique<wpi::DenseMap<
        int, std::weak_ptr<PneumaticsControlModule::DataStore>>[]>(numBuses);
  }

  return m_handleMaps[busId][module];
}

class PneumaticsControlModule::DataStore {
 public:
  explicit DataStore(int busId, int module, const char* stackTrace) {
    int32_t status = 0;
    HAL_CTREPCMHandle handle =
        HAL_InitializeCTREPCM(busId, module, stackTrace, &status);
    FRC_CheckErrorStatus(status, "Module {}", module);
    m_moduleObject = PneumaticsControlModule{busId, handle, module};
    m_moduleObject.m_dataStore =
        std::shared_ptr<DataStore>{this, wpi::NullDeleter<DataStore>()};
  }

  ~DataStore() noexcept { HAL_FreeCTREPCM(m_moduleObject.m_handle); }

  DataStore(DataStore&&) = delete;
  DataStore& operator=(DataStore&&) = delete;

 private:
  friend class PneumaticsControlModule;
  uint32_t m_reservedMask{0};
  bool m_compressorReserved{false};
  wpi::mutex m_reservedLock;
  PneumaticsControlModule m_moduleObject{0, HAL_InvalidHandle, 0};
};

PneumaticsControlModule::PneumaticsControlModule(int busId)
    : PneumaticsControlModule{busId, SensorUtil::GetDefaultCTREPCMModule()} {}

PneumaticsControlModule::PneumaticsControlModule(int busId, int module) {
  std::string stackTrace = wpi::GetStackTrace(1);
  std::scoped_lock lock(m_handleLock);
  auto& res = GetDataStore(busId, module);
  m_dataStore = res.lock();
  if (!m_dataStore) {
    m_dataStore =
        std::make_shared<DataStore>(busId, module, stackTrace.c_str());
    res = m_dataStore;
  }
  m_handle = m_dataStore->m_moduleObject.m_handle;
  m_module = module;
}

PneumaticsControlModule::PneumaticsControlModule(int /* busId */,
                                                 HAL_CTREPCMHandle handle,
                                                 int module)
    : m_handle{handle}, m_module{module} {}

bool PneumaticsControlModule::GetCompressor() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressor(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::DisableCompressor() {
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(m_handle, false, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticsControlModule::EnableCompressorDigital() {
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(m_handle, true, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticsControlModule::EnableCompressorAnalog(
    units::pounds_per_square_inch_t minPressure,
    units::pounds_per_square_inch_t maxPressure) {
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(m_handle, true, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticsControlModule::EnableCompressorHybrid(
    units::pounds_per_square_inch_t minPressure,
    units::pounds_per_square_inch_t maxPressure) {
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(m_handle, true, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

CompressorConfigType PneumaticsControlModule::GetCompressorConfigType() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMClosedLoopControl(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result ? CompressorConfigType::Digital
                : CompressorConfigType::Disabled;
}

bool PneumaticsControlModule::GetPressureSwitch() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMPressureSwitch(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

units::ampere_t PneumaticsControlModule::GetCompressorCurrent() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrent(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return units::ampere_t{result};
}

bool PneumaticsControlModule::GetCompressorCurrentTooHighFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrentTooHighFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorCurrentTooHighStickyFault() const {
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorShortedFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorShortedStickyFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedStickyFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorNotConnectedFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorNotConnectedFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorNotConnectedStickyFault() const {
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorNotConnectedStickyFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

bool PneumaticsControlModule::GetSolenoidVoltageFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetSolenoidVoltageStickyFault() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageStickyFault(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::ClearAllStickyFaults() {
  int32_t status = 0;
  HAL_ClearAllCTREPCMStickyFaults(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticsControlModule::SetSolenoids(int mask, int values) {
  int32_t status = 0;
  HAL_SetCTREPCMSolenoids(m_handle, mask, values, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

int PneumaticsControlModule::GetSolenoids() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoids(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

int PneumaticsControlModule::GetModuleNumber() const {
  return m_module;
}

int PneumaticsControlModule::GetSolenoidDisabledList() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidDisabledList(m_handle, &status);
  FRC_ReportError(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::FireOneShot(int index) {
  int32_t status = 0;
  HAL_FireCTREPCMOneShot(m_handle, index, &status);
  FRC_ReportError(status, "Module {}", m_module);
}

void PneumaticsControlModule::SetOneShotDuration(int index,
                                                 units::second_t duration) {
  int32_t status = 0;
  units::millisecond_t millis = duration;
  HAL_SetCTREPCMOneShotDuration(m_handle, index, millis.to<int32_t>(), &status);
  FRC_ReportError(status, "Module {}", m_module);
}

bool PneumaticsControlModule::CheckSolenoidChannel(int channel) const {
  return HAL_CheckCTREPCMSolenoidChannel(channel);
}

int PneumaticsControlModule::CheckAndReserveSolenoids(int mask) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  uint32_t uMask = static_cast<uint32_t>(mask);
  if ((m_dataStore->m_reservedMask & uMask) != 0) {
    return m_dataStore->m_reservedMask & uMask;
  }
  m_dataStore->m_reservedMask |= uMask;
  return 0;
}

void PneumaticsControlModule::UnreserveSolenoids(int mask) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_reservedMask &= ~(static_cast<uint32_t>(mask));
}

bool PneumaticsControlModule::ReserveCompressor() {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  if (m_dataStore->m_compressorReserved) {
    return false;
  }
  m_dataStore->m_compressorReserved = true;
  return true;
}

void PneumaticsControlModule::UnreserveCompressor() {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_compressorReserved = false;
}

units::volt_t PneumaticsControlModule::GetAnalogVoltage(int channel) const {
  return 0_V;
}

units::pounds_per_square_inch_t PneumaticsControlModule::GetPressure(
    int channel) const {
  return 0_psi;
}

Solenoid PneumaticsControlModule::MakeSolenoid(int channel) {
  return Solenoid{m_module, PneumaticsModuleType::CTREPCM, channel};
}

DoubleSolenoid PneumaticsControlModule::MakeDoubleSolenoid(int forwardChannel,
                                                           int reverseChannel) {
  return DoubleSolenoid{m_module, PneumaticsModuleType::CTREPCM, forwardChannel,
                        reverseChannel};
}

Compressor PneumaticsControlModule::MakeCompressor() {
  return Compressor{m_module, PneumaticsModuleType::CTREPCM};
}

void PneumaticsControlModule::ReportUsage(std::string_view device,
                                          std::string_view data) {
  HAL_ReportUsage(fmt::format("PCM[{}]/{}", m_module, device), data);
}

std::shared_ptr<PneumaticsBase> PneumaticsControlModule::GetForModule(
    int busId, int module) {
  std::string stackTrace = wpi::GetStackTrace(1);
  std::scoped_lock lock(m_handleLock);
  auto& res = GetDataStore(busId, module);
  std::shared_ptr<DataStore> dataStore = res.lock();
  if (!dataStore) {
    dataStore = std::make_shared<DataStore>(busId, module, stackTrace.c_str());
    res = dataStore;
  }

  return std::shared_ptr<PneumaticsBase>{dataStore, &dataStore->m_moduleObject};
}
