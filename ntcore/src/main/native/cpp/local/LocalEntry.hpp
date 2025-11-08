// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Handle.hpp"
#include "local/LocalSubscriber.hpp"
#include "wpi/util/Synchronization.h"

namespace wpi::nt::local {

struct LocalPublisher;

struct LocalEntry {
  static constexpr auto kType = Handle::kEntry;

  LocalEntry(NT_Entry handle, LocalSubscriber* subscriber)
      : handle{handle}, topic{subscriber->topic}, subscriber{subscriber} {}

  // invariants
  wpi::util::SignalObject<NT_Entry> handle;
  LocalTopic* topic;
  LocalSubscriber* subscriber;

  // the publisher (created on demand)
  LocalPublisher* publisher{nullptr};
};

}  // namespace wpi::nt::local
