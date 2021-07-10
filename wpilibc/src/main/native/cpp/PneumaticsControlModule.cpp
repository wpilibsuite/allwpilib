// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticsControlModule.h"

#include <hal/CTREPCM.h>
#include <wpi/StackTrace.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

wpi::mutex PneumaticsControlModule::m_handleLock;
wpi::DenseMap<int, std::weak_ptr<PneumaticsControlModule::DataStore>>
    PneumaticsControlModule::m_handleMap;

class PneumaticsControlModule::DataStore {
 public:
  explicit DataStore(int module, const char* stackTrace) {
    int32_t status = 0;
    HAL_CTREPCMHandle handle =
        HAL_InitializeCTREPCM(module, stackTrace, &status);
    FRC_CheckErrorStatus(status, "Module {}", module);
    m_moduleObject = PneumaticsControlModule{handle, module};
  }

  ~DataStore() noexcept { HAL_FreeCTREPCM(m_moduleObject.m_handle); }

  DataStore(DataStore&&) = delete;
  DataStore& operator=(DataStore&&) = delete;

 private:
  friend class PneumaticsControlModule;
  uint32_t m_reservedMask;
  wpi::mutex m_reservedLock;
  PneumaticsControlModule m_moduleObject{HAL_kInvalidHandle, 0};
};

PneumaticsControlModule::PneumaticsControlModule()
    : PneumaticsControlModule{SensorUtil::GetDefaultCTREPCMModule()} {}

PneumaticsControlModule::PneumaticsControlModule(int module) {
  std::string stackTrace = wpi::GetStackTrace(1);
  std::scoped_lock lock(m_handleLock);
  auto& res = m_handleMap[module];
  auto m_dataStore = res.lock();
  if (!m_dataStore) {
    m_dataStore = std::make_shared<DataStore>(module, stackTrace.c_str());
    res = m_dataStore;
  }
  m_handle = m_dataStore->m_moduleObject.m_handle;
  m_module = module;
}

PneumaticsControlModule::PneumaticsControlModule(HAL_CTREPCMHandle handle,
                                                 int module)
    : m_handle{handle}, m_module{module} {}

bool PneumaticsControlModule::GetCompressor() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressor(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::SetClosedLoopControl(bool enabled) {
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(m_handle, enabled, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

bool PneumaticsControlModule::GetClosedLoopControl() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMClosedLoopControl(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

bool PneumaticsControlModule::GetPressureSwitch() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMPressureSwitch(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

double PneumaticsControlModule::GetCompressorCurrent() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrent(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

bool PneumaticsControlModule::GetCompressorCurrentTooHighFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrentTooHighFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorCurrentTooHighStickyFault() {
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorShortedFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorShortedStickyFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedStickyFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorNotConnectedFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorNotConnectedFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetCompressorNotConnectedStickyFault() {
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorNotConnectedStickyFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

bool PneumaticsControlModule::GetSolenoidVoltageFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}
bool PneumaticsControlModule::GetSolenoidVoltageStickyFault() {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageStickyFault(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::ClearAllStickyFaults() {
  int32_t status = 0;
  HAL_ClearAllCTREPCMStickyFaults(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

void PneumaticsControlModule::SetSolenoids(int mask, int values) {
  int32_t status = 0;
  HAL_SetCTREPCMSolenoids(m_handle, mask, values, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

int PneumaticsControlModule::GetSolenoids() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoids(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

int PneumaticsControlModule::GetModuleNumber() const {
  return m_module;
}

int PneumaticsControlModule::GetSolenoidDisabledList() const {
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidDisabledList(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

void PneumaticsControlModule::FireOneShot(int index) {
  int32_t status = 0;
  HAL_FireCTREPCMOneShot(m_handle, index, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

void PneumaticsControlModule::SetOneShotDuration(int index,
                                                 units::second_t duration) {
  int32_t status = 0;
  units::millisecond_t millis = duration;
  HAL_SetCTREPCMOneShotDuration(m_handle, index, millis.to<int32_t>(), &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
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

std::shared_ptr<PneumaticsBase> PneumaticsControlModule::Duplicate() {
  return std::shared_ptr<PneumaticsBase>{m_dataStore,
                                         &m_dataStore->m_moduleObject};
}

// void PneumaticsControlModule::InitSendable(wpi::SendableBuilder& builder) {
//   builder.SetSmartDashboardType("Compressor");
//   builder.AddBooleanProperty(
//       "Closed Loop Control", [=]() { return GetClosedLoopControl(); },
//       [=](bool value) { SetClosedLoopControl(value); });
//   builder.AddBooleanProperty(
//       "Enabled", [=] { return GetCompressor(); }, nullptr);
//   builder.AddBooleanProperty(
//       "Pressure switch", [=]() { return GetPressureSwitch(); }, nullptr);
// }
