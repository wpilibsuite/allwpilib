// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PubSubOptions.h"

#include "ntcore_cpp.h"

using namespace nt;

nt::PubSubOptions::PubSubOptions(wpi::span<const PubSubOption> options) {
  for (auto&& option : options) {
    switch (option.type) {
      case NT_PUBSUB_PERIODIC:
        periodic = option.value;
        break;
      case NT_PUBSUB_SENDALL:
        sendAll = option.value != 0;
        if (sendAll) {
          pollStorageSize = 20;
        }
        break;
      case NT_PUBSUB_TOPICSONLY:
        topicsOnly = option.value != 0;
        break;
      case NT_PUBSUB_KEEPDUPLICATES:
        keepDuplicates = option.value != 0;
        break;
      case NT_PUBSUB_POLLSTORAGE:
        pollStorageSize = static_cast<size_t>(option.value);
        break;
      default:
        break;
    }
  }
}
