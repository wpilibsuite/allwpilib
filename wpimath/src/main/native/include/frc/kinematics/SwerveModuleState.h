// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"
#include "units/math.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents the state of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  units::meters_per_second_t speed = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModuleState and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const SwerveModuleState& other) const;

  /**
   * Minimize the change in heading the desired swerve module state would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * Note: This method should not be used together with the CosineCompensation method. 
   * Both methods perform angle optimization, and using them together is redundant.
   *
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   */
  static SwerveModuleState Optimize(const SwerveModuleState& desiredState,
                                    const Rotation2d& currentAngle);

  /**
   * Scales the module speed by the cosine of the angle error. This reduces skew caused by changing direction.
   * 
   * For example, if the current angle of the module matches the desired angle (i.e., there is no error), 
   * the speed of the module remains unchanged as cos(0) = 1. However, if the current angle is 90 degrees off 
   * from the desired angle, the speed of the module becomes 0 as cos(90°) = 0. This means the module will stop 
   * moving, allowing it to correct its angle without moving in the wrong direction.
   * 
   * Note: This method should not be used together with the Optimize method.
   * Both methods perform angle optimization, and using them together is redundant.
   * 
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   * @return The cosine compensated swerve module state.
   */
   static SwerveModuleState CosineCompensation(const SwerveModuleState& desiredState, const Rotation2d& currentAngle);
};
}  // namespace frc

#include "frc/kinematics/proto/SwerveModuleStateProto.h"
#include "frc/kinematics/struct/SwerveModuleStateStruct.h"
