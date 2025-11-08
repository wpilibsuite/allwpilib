// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/nt/ntcore_cpp.hpp"

namespace nt {

// internal helper class for PubSubOptions
class PubSubOptionsImpl : public PubSubOptions {
 public:
  constexpr PubSubOptionsImpl() : PubSubOptionsImpl{kDefaultPubSubOptions} {}

  /*implicit*/ constexpr PubSubOptionsImpl(  // NOLINT
      const PubSubOptions& options)
      : PubSubOptions{options} {
    if (periodic == 0) {
      periodic = kDefaultPeriodic;
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

  static constexpr unsigned int kDefaultPeriodicMs = 100;
  unsigned int periodicMs = kDefaultPeriodicMs;
};

}  // namespace nt
