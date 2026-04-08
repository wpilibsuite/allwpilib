// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHub.hpp"

#include <memory>

#include "wpi/hal/UsageReporting.hpp"
#include "wpi/hardware/expansionhub/ExpansionHubMotor.hpp"
#include "wpi/hardware/expansionhub/ExpansionHubServo.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/SystemServer.hpp"

using namespace wpi;

wpi::util::mutex ExpansionHub::m_handleLock;
std::weak_ptr<ExpansionHub::DataStore> ExpansionHub::m_storeMap[4];

class ExpansionHub::DataStore {
 public:
  explicit DataStore(int usbId) : m_usbId{usbId} {
    auto systemServer = SystemServer::GetSystemServer();

    m_hubConnectedSubscriber =
        systemServer.GetBooleanTopic(fmt::format("/rhsp/{}/connected", usbId))
            .Subscribe(false);
  }

  DataStore(DataStore&) = delete;
  DataStore(DataStore&&) = delete;
  DataStore& operator=(DataStore&) = delete;
  DataStore& operator=(DataStore&&) = delete;

  wpi::nt::BooleanSubscriber m_hubConnectedSubscriber;

  uint32_t m_reservedMotorMask{0};
  uint32_t m_reservedServoMask{0};
  wpi::util::mutex m_reservedLock;

  int m_usbId;
};

std::shared_ptr<ExpansionHub::DataStore> ExpansionHub::GetForUsbId(int usbId) {
  WPILIB_AssertMessage(usbId >= 0 && usbId < NumUsbPorts, "USB {} out of range",
                       usbId);
  std::scoped_lock lock{m_handleLock};
  std::weak_ptr<DataStore>& weakStore = m_storeMap[usbId];
  auto strongStore = weakStore.lock();
  if (!strongStore) {
    strongStore = std::make_shared<DataStore>(usbId);
    weakStore = strongStore;
  }
  return strongStore;
}

ExpansionHub::ExpansionHub(int usbId) : m_dataStore{GetForUsbId(usbId)} {}

ExpansionHubServo ExpansionHub::MakeServo(int channel) {
  return ExpansionHubServo{m_dataStore->m_usbId, channel};
}

ExpansionHubMotor ExpansionHub::MakeMotor(int channel) {
  return ExpansionHubMotor{m_dataStore->m_usbId, channel};
}

bool ExpansionHub::IsHubConnected() const {
  return m_dataStore->m_hubConnectedSubscriber.Get(false);
}

bool ExpansionHub::CheckAndReserveServo(int channel) {
  int mask = 1 << channel;
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  if ((m_dataStore->m_reservedServoMask & mask) != 0) {
    return false;
  }
  m_dataStore->m_reservedServoMask |= mask;
  return true;
}

void ExpansionHub::UnreserveServo(int channel) {
  int mask = 1 << channel;
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_reservedServoMask &= ~mask;
}

bool ExpansionHub::CheckAndReserveMotor(int channel) {
  int mask = 1 << channel;
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  if ((m_dataStore->m_reservedMotorMask & mask) != 0) {
    return false;
  }
  m_dataStore->m_reservedMotorMask |= mask;
  return true;
}

void ExpansionHub::UnreserveMotor(int channel) {
  int mask = 1 << channel;
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->m_reservedMotorMask &= ~mask;
}

void ExpansionHub::ReportUsage(std::string_view device, std::string_view data) {
  HAL_ReportUsage(
      fmt::format("ExpansionHub[{}]/{}", m_dataStore->m_usbId, device), data);
}
