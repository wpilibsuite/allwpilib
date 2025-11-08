// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "wpi/util/Synchronization.h"

#include "Handle.hpp"
#include "Types_internal.hpp"
#include "ValueCircularBuffer.hpp"
#include "VectorSet.hpp"
#include "local/LocalTopic.hpp"
#include "local/PubSubConfig.hpp"
#include "wpi/nt/ntcore_c.h"

namespace nt::local {

struct LocalSubscriber {
  static constexpr auto kType = Handle::kSubscriber;

  LocalSubscriber(NT_Subscriber handle, LocalTopic* topic, PubSubConfig config)
      : handle{handle},
        topic{topic},
        config{std::move(config)},
        pollStorage{config.pollStorage} {}

  void UpdateActive() {
    // for subscribers, unassigned is a wildcard
    // also allow numerically compatible subscribers
    active = config.type == NT_UNASSIGNED ||
             (config.type == topic->type && config.typeStr == topic->typeStr) ||
             IsNumericCompatible(config.type, topic->type);
  }

  // invariants
  wpi::SignalObject<NT_Subscriber> handle;
  LocalTopic* topic;
  PubSubConfig config;

  // whether or not the subscriber should actually receive values
  bool active{false};

  // polling storage
  ValueCircularBuffer pollStorage;

  // value listeners
  VectorSet<NT_Listener> valueListeners;
};

}  // namespace nt::local
