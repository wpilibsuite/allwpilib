// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string_view>

#include "ntcore_cpp.h"

namespace nt {

class IListenerStorage {
 public:
  // Return false if event should not be issued (final check).
  // This is called only during Notify() processing.
  using FinishEventFunc = std::function<bool(unsigned int mask, Event* event)>;

  virtual ~IListenerStorage() = default;

  virtual void Activate(NT_Listener listener, unsigned int mask,
                        FinishEventFunc finishEvent = {}) = 0;

  // If handles is not empty, notifies ONLY those listeners
  virtual void Notify(std::span<const NT_Listener> handles, unsigned int flags,
                      std::span<ConnectionInfo const* const> infos) = 0;
  virtual void Notify(std::span<const NT_Listener> handles, unsigned int flags,
                      std::span<const TopicInfo> infos) = 0;
  virtual void Notify(std::span<const NT_Listener> handles, unsigned int flags,
                      NT_Topic topic, NT_Handle subentry,
                      const Value& value) = 0;
  virtual void Notify(unsigned int flags, unsigned int level,
                      std::string_view filename, unsigned int line,
                      std::string_view message) = 0;

  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              const ConnectionInfo* info) {
    Notify(handles, flags, {&info, 1});
  }
  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              const TopicInfo& info) {
    Notify(handles, flags, {&info, 1});
  }
};

}  // namespace nt
