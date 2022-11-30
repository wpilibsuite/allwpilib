// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PubSubOptions.h"

#include "ntcore_cpp.h"

using namespace nt;

nt::PubSubOptions::PubSubOptions(std::span<const PubSubOption> options) {
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
      case NT_PUBSUB_LOCALREMOTE:
        switch (static_cast<int>(option.value)) {
          case 0:
          case 3:
            fromLocal = true;
            fromRemote = true;
            break;
          case 1:
            fromLocal = true;
            fromRemote = false;
            break;
          case 2:
            fromLocal = false;
            fromRemote = true;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}
