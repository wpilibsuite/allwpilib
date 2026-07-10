// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "PubSubOptions.hpp"
#include "TestPrinters.hpp"

namespace wpi::nt {

inline bool operator==(const PubSubOptionsImpl& lhs,
                       const PubSubOptionsImpl& rhs) {
  return lhs.periodicMs == rhs.periodicMs &&
         lhs.pollStorage == rhs.pollStorage && lhs.sendAll == rhs.sendAll &&
         lhs.keepDuplicates == rhs.keepDuplicates &&
         lhs.topicsOnly == rhs.topicsOnly &&
         lhs.prefixMatch == rhs.prefixMatch &&
         lhs.disableSignal == rhs.disableSignal;
}

}  // namespace wpi::nt
