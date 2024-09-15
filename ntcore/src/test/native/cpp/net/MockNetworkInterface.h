// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/json.h>

#include "PubSubOptions.h"
#include "gmock/gmock.h"
#include "net/NetworkInterface.h"

namespace nt::net {

class MockLocalInterface : public LocalInterface {
 public:
  MOCK_METHOD(NT_Topic, NetworkAnnounce,
              (std::string_view name, std::string_view typeStr,
               const wpi::json& properties, std::optional<int> pubuid),
              (override));
  MOCK_METHOD(void, NetworkUnannounce, (std::string_view name), (override));
  MOCK_METHOD(void, NetworkPropertiesUpdate,
              (std::string_view name, const wpi::json& update, bool ack),
              (override));
  MOCK_METHOD(void, NetworkSetValue, (NT_Topic topicHandle, const Value& value),
              (override));
};

class MockNetworkInterface : public NetworkInterface {
 public:
  MOCK_METHOD(void, Publish,
              (int pubuid, std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptionsImpl& options),
              (override));
  MOCK_METHOD(void, Unpublish, (int pubuid), (override));
  MOCK_METHOD(void, SetProperties,
              (std::string_view name, const wpi::json& update), (override));
  MOCK_METHOD(void, Subscribe,
              (int subuid, std::span<const std::string> prefixes,
               const PubSubOptionsImpl& options),
              (override));
  MOCK_METHOD(void, Unsubscribe, (int subuid), (override));
  MOCK_METHOD(void, SetValue, (int pubuid, const Value& value), (override));
};

class MockLocalStorage : public ILocalStorage {
 public:
  MOCK_METHOD(NT_Topic, NetworkAnnounce,
              (std::string_view name, std::string_view typeStr,
               const wpi::json& properties, std::optional<int> pubuid),
              (override));
  MOCK_METHOD(void, NetworkUnannounce, (std::string_view name), (override));
  MOCK_METHOD(void, NetworkPropertiesUpdate,
              (std::string_view name, const wpi::json& update, bool ack),
              (override));
  MOCK_METHOD(void, NetworkSetValue, (NT_Topic topicHandle, const Value& value),
              (override));
  MOCK_METHOD(void, StartNetwork, (NetworkInterface * network), (override));
  MOCK_METHOD(void, ClearNetwork, (), (override));
};

}  // namespace nt::net
