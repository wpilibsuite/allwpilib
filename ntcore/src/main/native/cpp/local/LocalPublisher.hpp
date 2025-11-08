// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "wpi/util/Synchronization.h"

#include "Handle.hpp"
#include "local/LocalTopic.hpp"
#include "local/PubSubConfig.hpp"

namespace nt::local {

struct LocalPublisher {
  static constexpr auto kType = Handle::kPublisher;

  LocalPublisher(NT_Publisher handle, LocalTopic* topic, PubSubConfig config)
      : handle{handle}, topic{topic}, config{std::move(config)} {}

  void UpdateActive() {
    active = config.type == topic->type && config.typeStr == topic->typeStr;
  }

  // invariants
  wpi::SignalObject<NT_Publisher> handle;
  LocalTopic* topic;
  PubSubConfig config;

  // whether or not the publisher should actually publish values
  bool active{false};
};

}  // namespace nt::local
