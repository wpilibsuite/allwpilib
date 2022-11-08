// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "IListenerStorage.h"
#include "gmock/gmock.h"

namespace nt {

class MockListenerStorage : public IListenerStorage {
 public:
  MOCK_METHOD(void, Activate,
              (NT_Listener listenerHandle, unsigned int mask,
               FinishEventFunc finishEvent),
              (override));
  MOCK_METHOD(void, Notify,
              (std::span<const NT_Listener> handles, unsigned int flags,
               std::span<ConnectionInfo const* const> infos),
              (override));
  MOCK_METHOD(void, Notify,
              (std::span<const NT_Listener> handles, unsigned int flags,
               std::span<const TopicInfo> infos),
              (override));
  MOCK_METHOD(void, Notify,
              (std::span<const NT_Listener> handles, unsigned int flags,
               NT_Topic topic, NT_Handle subentry, const Value& value),
              (override));
  MOCK_METHOD(void, Notify,
              (unsigned int flags, unsigned int level,
               std::string_view filename, unsigned int line,
               std::string_view message),
              (override));
};

}  // namespace nt
