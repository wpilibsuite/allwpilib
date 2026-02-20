// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

/**
 * The type of robot match that the robot is part of.
 */
enum class MatchType {
  /// None.
  kNone,
  /// Practice.
  kPractice,
  /// Qualification.
  kQualification,
  /// Elimination.
  kElimination
};

}  // namespace wpi
