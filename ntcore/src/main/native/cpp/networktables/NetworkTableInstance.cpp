// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NetworkTableInstance.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/util/SmallVector.hpp>
#include <wpi/util/print.hpp>

#include "wpi/nt/BooleanArrayTopic.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/FloatArrayTopic.hpp"
#include "wpi/nt/FloatTopic.hpp"
#include "wpi/nt/IntegerArrayTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/RawTopic.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StringTopic.hpp"

using namespace wpi::nt;

Topic NetworkTableInstance::GetTopic(std::string_view name) const {
  return Topic{::wpi::nt::GetTopic(m_handle, name)};
}

BooleanTopic NetworkTableInstance::GetBooleanTopic(
    std::string_view name) const {
  return BooleanTopic{GetTopic(name)};
}

IntegerTopic NetworkTableInstance::GetIntegerTopic(
    std::string_view name) const {
  return IntegerTopic{GetTopic(name)};
}

FloatTopic NetworkTableInstance::GetFloatTopic(std::string_view name) const {
  return FloatTopic{GetTopic(name)};
}

DoubleTopic NetworkTableInstance::GetDoubleTopic(std::string_view name) const {
  return DoubleTopic{GetTopic(name)};
}

StringTopic NetworkTableInstance::GetStringTopic(std::string_view name) const {
  return StringTopic{GetTopic(name)};
}

RawTopic NetworkTableInstance::GetRawTopic(std::string_view name) const {
  return RawTopic{GetTopic(name)};
}

BooleanArrayTopic NetworkTableInstance::GetBooleanArrayTopic(
    std::string_view name) const {
  return BooleanArrayTopic{GetTopic(name)};
}

IntegerArrayTopic NetworkTableInstance::GetIntegerArrayTopic(
    std::string_view name) const {
  return IntegerArrayTopic{GetTopic(name)};
}

FloatArrayTopic NetworkTableInstance::GetFloatArrayTopic(
    std::string_view name) const {
  return FloatArrayTopic{GetTopic(name)};
}

DoubleArrayTopic NetworkTableInstance::GetDoubleArrayTopic(
    std::string_view name) const {
  return DoubleArrayTopic{GetTopic(name)};
}

StringArrayTopic NetworkTableInstance::GetStringArrayTopic(
    std::string_view name) const {
  return StringArrayTopic{GetTopic(name)};
}

std::shared_ptr<NetworkTable> NetworkTableInstance::GetTable(
    std::string_view key) const {
  if (key.empty() || key == "/") {
    return std::make_shared<NetworkTable>(m_handle, "",
                                          NetworkTable::private_init{});
  } else if (key.front() == NetworkTable::PATH_SEPARATOR_CHAR) {
    return std::make_shared<NetworkTable>(m_handle, key,
                                          NetworkTable::private_init{});
  } else {
    return std::make_shared<NetworkTable>(m_handle, fmt::format("/{}", key),
                                          NetworkTable::private_init{});
  }
}

void NetworkTableInstance::SetServer(std::span<const std::string_view> servers,
                                     unsigned int port) {
  std::vector<std::pair<std::string_view, unsigned int>> serversArr;
  serversArr.reserve(servers.size());
  for (const auto& server : servers) {
    serversArr.emplace_back(std::string{server}, port);
  }
  SetServer(serversArr);
}

NT_Listener NetworkTableInstance::AddListener(Topic topic,
                                              unsigned int eventMask,
                                              ListenerCallback listener) {
  if (::wpi::nt::GetInstanceFromHandle(topic.GetHandle()) != m_handle) {
    wpi::util::print(stderr, "AddListener: topic is not from this instance\n");
    return 0;
  }
  return ::wpi::nt::AddListener(topic.GetHandle(), eventMask, std::move(listener));
}

NT_Listener NetworkTableInstance::AddListener(Subscriber& subscriber,
                                              unsigned int eventMask,
                                              ListenerCallback listener) {
  if (::wpi::nt::GetInstanceFromHandle(subscriber.GetHandle()) != m_handle) {
    wpi::util::print(stderr, "AddListener: subscriber is not from this instance\n");
    return 0;
  }
  return ::wpi::nt::AddListener(subscriber.GetHandle(), eventMask,
                           std::move(listener));
}

NT_Listener NetworkTableInstance::AddListener(const NetworkTableEntry& entry,
                                              int eventMask,
                                              ListenerCallback listener) {
  if (::wpi::nt::GetInstanceFromHandle(entry.GetHandle()) != m_handle) {
    wpi::util::print(stderr, "AddListener: entry is not from this instance\n");
    return 0;
  }
  return ::wpi::nt::AddListener(entry.GetHandle(), eventMask, std::move(listener));
}

NT_Listener NetworkTableInstance::AddListener(MultiSubscriber& subscriber,
                                              int eventMask,
                                              ListenerCallback listener) {
  if (::wpi::nt::GetInstanceFromHandle(subscriber.GetHandle()) != m_handle) {
    wpi::util::print(stderr, "AddListener: subscriber is not from this instance\n");
    return 0;
  }
  return ::wpi::nt::AddListener(subscriber.GetHandle(), eventMask,
                           std::move(listener));
}
