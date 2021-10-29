// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PneumaticHub.h"

#include <hal/REVPH.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>

#include "frc/Compressor.h"
#include "frc/DoubleSolenoid.h"
#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/Solenoid.h"

using namespace frc;

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
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

void PneumaticHub::SetClosedLoopControl(bool enabled) {
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControl(m_handle, enabled, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

bool PneumaticHub::GetClosedLoopControl() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHClosedLoopControl(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

bool PneumaticHub::GetPressureSwitch() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHPressureSwitch(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

double PneumaticHub::GetCompressorCurrent() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressorCurrent(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

void PneumaticHub::SetSolenoids(int mask, int values) {
  int32_t status = 0;
  HAL_SetREVPHSolenoids(m_handle, mask, values, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
}

int PneumaticHub::GetSolenoids() const {
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoids(m_handle, &status);
  FRC_CheckErrorStatus(status, "Module {}", m_module);
  return result;
}

int PneumaticHub::GetModuleNumber() const {
  return m_module;
}

int PneumaticHub::GetSolenoidDisabledList() const {
  return 0;
  // TODO Fix me
  // int32_t status = 0;
  // auto result = HAL_GetREVPHSolenoidDisabledList(m_handle, &status);
  // FRC_CheckErrorStatus(status, "Module {}", m_module);
  // return result;
}

void PneumaticHub::FireOneShot(int index) {
  // TODO Fix me
  // int32_t status = 0;
  // HAL_FireREVPHOneShot(m_handle, index, &status);
  // FRC_CheckErrorStatus(status, "Module {}", m_module);
}

void PneumaticHub::SetOneShotDuration(int index, units::second_t duration) {
  // TODO Fix me
  // int32_t status = 0;
  // units::millisecond_t millis = duration;
  // HAL_SetREVPHOneShotDuration(m_handle, index, millis.to<int32_t>(),
  // &status); FRC_CheckErrorStatus(status, "Module {}", m_module);
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
