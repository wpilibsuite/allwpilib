// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHub.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "wpi/framework/RobotBase.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/hardware/expansionhub/ExpansionHubCRServo.hpp"
#include "wpi/hardware/expansionhub/ExpansionHubMotor.hpp"
#include "wpi/hardware/expansionhub/ExpansionHubServo.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/SystemServer.hpp"
#include "wpi/system/Timer.hpp"

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

    // Wait up to half a second for connected to come up, using a poll loop to
    // ensure we don't block.
    if constexpr (RobotBase::IsReal()) {
      auto startTime = Timer::GetMonotonicTimestamp();
      while (Timer::GetMonotonicTimestamp() - startTime < 0.5_s) {
        if (m_hubConnectedSubscriber.Get(false)) {
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }

  DataStore(DataStore&) = delete;
  DataStore(DataStore&&) = delete;
  DataStore& operator=(DataStore&) = delete;
  DataStore& operator=(DataStore&&) = delete;

  void validateFollowerConfiguration();
  void validateRootFollower(int baseChannel, int channel,
                            std::array<int, NumMotorPorts>& followerVisited);
  std::string getFollowerStringCycle(
      int baseChannel, std::array<int, NumMotorPorts>& followerVisited);

  wpi::nt::BooleanSubscriber m_hubConnectedSubscriber;

  uint32_t m_reservedMotorMask{0};
  uint32_t m_reservedServoMask{0};
  wpi::util::mutex m_reservedLock;

  std::optional<int> followerConfiguration[NumMotorPorts];

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

ExpansionHubCRServo ExpansionHub::MakeCRServo(int channel) {
  return ExpansionHubCRServo{m_dataStore->m_usbId, channel};
}

bool ExpansionHub::IsHubConnected() const {
  return m_dataStore->m_hubConnectedSubscriber.Get(false);
}

int ExpansionHub::GetUsbId() const {
  return m_dataStore->m_usbId;
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
  m_dataStore->followerConfiguration[channel].reset();
}

void ExpansionHub::ReportUsage(std::string_view device, std::string_view data) {
  HAL_ReportUsage(
      fmt::format("ExpansionHub[{}]/{}", m_dataStore->m_usbId, device), data);
}

std::string ExpansionHub::DataStore::getFollowerStringCycle(
    int baseChannel, std::array<int, NumMotorPorts>& followerVisited) {
  std::string result = fmt::format("{}", baseChannel);
  int current = baseChannel;
  while (followerVisited[current] != baseChannel) {
    current = followerVisited[current];
    result += fmt::format(" -> {}", current);
  }
  result += fmt::format(" -> {}", followerVisited[current]);
  return result;
}

void ExpansionHub::DataStore::validateRootFollower(
    int baseChannel, int channel,
    std::array<int, NumMotorPorts>& followerVisited) {
  if (followerVisited[channel] != -1) {
    throw WPILIB_MakeError(
        err::ParameterOutOfRange, "Follower cycle detected on hub {}: {}",
        m_usbId, getFollowerStringCycle(baseChannel, followerVisited));
  }
  auto leader = followerConfiguration[channel];
  if (!leader.has_value()) {
    return;
  }
  followerVisited[channel] = *leader;
  validateRootFollower(baseChannel, *leader, followerVisited);
}

void ExpansionHub::DataStore::validateFollowerConfiguration() {
  std::array<int, NumMotorPorts> followerVisited;
  for (int i = 0; i < NumMotorPorts; i++) {
    for (int j = 0; j < NumMotorPorts; j++) {
      followerVisited[j] = -1;
    }
    validateRootFollower(i, i, followerVisited);
  }
}

void ExpansionHub::AddFollower(int leaderChannel, int followerChannel) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->followerConfiguration[followerChannel] = leaderChannel;
  m_dataStore->validateFollowerConfiguration();
}

void ExpansionHub::RemoveFollower(int followerChannel) {
  std::scoped_lock lock{m_dataStore->m_reservedLock};
  m_dataStore->followerConfiguration[followerChannel].reset();
}
