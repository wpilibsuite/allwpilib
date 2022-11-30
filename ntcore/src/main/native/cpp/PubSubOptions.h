// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#include "ntcore_cpp.h"

namespace nt {

// options built from array of PubSubOption
class PubSubOptions {
 public:
  PubSubOptions() = default;
  explicit PubSubOptions(std::span<const PubSubOption> options);

  double periodic = 0.1;
  size_t pollStorageSize = 1;
  bool sendAll = false;
  bool topicsOnly = false;
  bool prefixMatch = false;
  bool keepDuplicates = false;
  bool fromRemote = true;
  bool fromLocal = true;
};

}  // namespace nt
