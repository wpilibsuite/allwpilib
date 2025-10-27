// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/json.hpp>

#include "gmock/gmock.h"
#include "net/NetworkInterface.hpp"

namespace wpi::nt::net {

class MockLocalStorage : public ILocalStorage {
 public:
  MOCK_METHOD(int, ServerAnnounce,
              (std::string_view name, int id, std::string_view typeStr,
               const wpi::util::json& properties, std::optional<int> pubuid),
              (override));
  MOCK_METHOD(void, ServerUnannounce, (std::string_view name, int id),
              (override));
  MOCK_METHOD(void, ServerPropertiesUpdate,
              (std::string_view name, const wpi::util::json& update, bool ack),
              (override));
  MOCK_METHOD(void, ServerSetValue, (int topicId, const Value& value),
              (override));
  MOCK_METHOD(void, StartNetwork, (ClientMessageHandler * network), (override));
  MOCK_METHOD(void, ClearNetwork, (), (override));
};

}  // namespace wpi::nt::net
