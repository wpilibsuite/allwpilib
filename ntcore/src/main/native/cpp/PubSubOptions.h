// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "ntcore_cpp.h"

namespace nt {

// internal helper class for PubSubOptions
class PubSubOptionsImpl : public PubSubOptions {
 public:
  constexpr PubSubOptionsImpl() : PubSubOptionsImpl{DEFAULT_PUB_SUB_OPTIONS} {}

  /*implicit*/ constexpr PubSubOptionsImpl(  // NOLINT
      const PubSubOptions& options)
      : PubSubOptions{options} {
    if (periodic == 0) {
      periodic = DEFAULT_PERIODIC;
    }
    periodicMs = static_cast<unsigned int>(periodic * 1000);
    if (pollStorage == 0) {
      if (sendAll) {
        pollStorage = 20;
      } else {
        pollStorage = 1;
      }
    }
  }

  static constexpr unsigned int DEFAULT_PERIODIC_MS = 100;
  unsigned int periodicMs = DEFAULT_PERIODIC_MS;
};

}  // namespace nt
