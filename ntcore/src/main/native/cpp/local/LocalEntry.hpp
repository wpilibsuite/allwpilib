// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Synchronization.h>

#include "Handle.h"
#include "local/LocalSubscriber.h"

namespace nt::local {

struct LocalPublisher;

struct LocalEntry {
  static constexpr auto kType = Handle::kEntry;

  LocalEntry(NT_Entry handle, LocalSubscriber* subscriber)
      : handle{handle}, topic{subscriber->topic}, subscriber{subscriber} {}

  // invariants
  wpi::SignalObject<NT_Entry> handle;
  LocalTopic* topic;
  LocalSubscriber* subscriber;

  // the publisher (created on demand)
  LocalPublisher* publisher{nullptr};
};

}  // namespace nt::local
