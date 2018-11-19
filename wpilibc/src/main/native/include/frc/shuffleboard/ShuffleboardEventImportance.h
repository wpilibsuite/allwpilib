/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

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
