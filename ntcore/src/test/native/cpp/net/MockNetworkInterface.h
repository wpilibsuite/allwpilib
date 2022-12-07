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
               const wpi::json& properties, NT_Publisher pubHandle),
              (override));
  MOCK_METHOD(void, NetworkUnannounce, (std::string_view name), (override));
  MOCK_METHOD(void, NetworkPropertiesUpdate,
              (std::string_view name, const wpi::json& update, bool ack),
              (override));
  MOCK_METHOD(void, NetworkSetValue, (NT_Topic topicHandle, const Value& value),
              (override));
};

class MockNetworkStartupInterface : public NetworkStartupInterface {
 public:
  MOCK_METHOD(void, Publish,
              (NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options),
              (override));
  MOCK_METHOD(void, Subscribe,
              (NT_Subscriber subHandle, std::span<const std::string> prefixes,
               const PubSubOptions& options),
              (override));
  MOCK_METHOD(void, SetValue, (NT_Publisher pubHandle, const Value& value),
              (override));
};

class MockNetworkInterface : public NetworkInterface {
 public:
  MOCK_METHOD(void, Publish,
              (NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options),
              (override));
  MOCK_METHOD(void, Unpublish, (NT_Publisher pubHandle, NT_Topic topicHandle),
              (override));
  MOCK_METHOD(void, SetProperties,
              (NT_Topic topicHandle, std::string_view name,
               const wpi::json& update),
              (override));
  MOCK_METHOD(void, Subscribe,
              (NT_Subscriber subHandle, std::span<const std::string> prefixes,
               const PubSubOptions& options),
              (override));
  MOCK_METHOD(void, Unsubscribe, (NT_Subscriber subHandle), (override));
  MOCK_METHOD(void, SetValue, (NT_Publisher pubHandle, const Value& value),
              (override));
};

class MockLocalStorage : public ILocalStorage {
 public:
  MOCK_METHOD(NT_Topic, NetworkAnnounce,
              (std::string_view name, std::string_view typeStr,
               const wpi::json& properties, NT_Publisher pubHandle),
              (override));
  MOCK_METHOD(void, NetworkUnannounce, (std::string_view name), (override));
  MOCK_METHOD(void, NetworkPropertiesUpdate,
              (std::string_view name, const wpi::json& update, bool ack),
              (override));
  MOCK_METHOD(void, NetworkSetValue, (NT_Topic topicHandle, const Value& value),
              (override));
  MOCK_METHOD(void, StartNetwork,
              (NetworkStartupInterface & startup, NetworkInterface* network),
              (override));
  MOCK_METHOD(void, ClearNetwork, (), (override));
};

}  // namespace nt::net
