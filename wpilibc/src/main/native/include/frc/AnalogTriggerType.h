// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/** Defines the state in which the AnalogTrigger triggers. */
enum class AnalogTriggerType {
  /// In window.
  kInWindow = 0,
  /// State.
  kState = 1,
  /// Rising Pulse.
  kRisingPulse = 2,
  /// Falling pulse.
  kFallingPulse = 3
};

}  // namespace frc
