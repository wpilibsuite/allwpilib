// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/json.h>

#include "PubSubOptions.h"
#include "gmock/gmock.h"
#include "net/MessageHandler.h"

namespace nt::net {

class MockClientMessageHandler : public net::ClientMessageHandler {
 public:
  MOCK_METHOD(void, ClientPublish,
              (int pubuid, std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptionsImpl& options),
              (override));
  MOCK_METHOD(void, ClientUnpublish, (int pubuid), (override));
  MOCK_METHOD(void, ClientSetProperties,
              (std::string_view name, const wpi::json& update), (override));
  MOCK_METHOD(void, ClientSubscribe,
              (int subuid, std::span<const std::string> prefixes,
               const PubSubOptionsImpl& options),
              (override));
  MOCK_METHOD(void, ClientUnsubscribe, (int subuid), (override));
  MOCK_METHOD(void, ClientSetValue, (int pubuid, const Value& value),
              (override));
};

class MockServerMessageHandler : public net::ServerMessageHandler {
 public:
  MOCK_METHOD(int, ServerAnnounce,
              (std::string_view name, int id, std::string_view typeStr,
               const wpi::json& properties, std::optional<int> pubuid),
              (override));
  MOCK_METHOD(void, ServerUnannounce, (std::string_view name, int id),
              (override));
  MOCK_METHOD(void, ServerPropertiesUpdate,
              (std::string_view name, const wpi::json& update, bool ack),
              (override));
  MOCK_METHOD(void, ServerSetValue, (int topicuid, const Value& value),
              (override));
};

}  // namespace nt::net
