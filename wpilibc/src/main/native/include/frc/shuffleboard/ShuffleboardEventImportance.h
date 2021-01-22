// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

namespace frc {

// Maintainer note: this enum is mirrored in WPILibJ and in Shuffleboard
// Modifying the enum or enum strings requires a corresponding change to the
// Java enum and the enum in Shuffleboard

enum ShuffleboardEventImportance { kTrivial, kLow, kNormal, kHigh, kCritical };

inline wpi::StringRef ShuffleboardEventImportanceName(
    ShuffleboardEventImportance importance) {
  switch (importance) {
    case kTrivial:
      return "TRIVIAL";
    case kLow:
      return "LOW";
    case kNormal:
      return "NORMAL";
    case kHigh:
      return "HIGH";
    case kCritical:
      return "CRITICAL";
    default:
      return "NORMAL";
  }
}

}  // namespace frc
