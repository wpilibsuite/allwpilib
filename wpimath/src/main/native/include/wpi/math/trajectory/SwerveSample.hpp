// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/math/kinematics/SwerveModuleState.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a swerve drive trajectory.
 */
class WPILIB_DLLEXPORT SwerveSample : public TrajectorySample<SwerveSample> {
 public:
  /**
   * The states of the wheels in the robot's reference frame, in the order that
   * each wheel is specified in the SwerveDriveKinematics object.
   */
  std::vector<SwerveModuleState> states;

  constexpr SwerveSample() = default;

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param states The swerve module states at this sample.
   */
  SwerveSample(wpi::units::second_t timestamp, const Pose2d& pose,
               const ChassisSpeeds& velocity,
               const ChassisAccelerations& acceleration,
               std::initializer_list<SwerveModuleState> states)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        states{states} {}

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param states The swerve module states at this sample.
   */
  SwerveSample(wpi::units::second_t timestamp, const Pose2d& pose,
               const ChassisSpeeds& velocity,
               const ChassisAccelerations& acceleration,
               std::span<const SwerveModuleState> states)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        states{states.begin(), states.end()} {}

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param states The swerve module states at this sample.
   */
  SwerveSample(wpi::units::second_t timestamp, const Pose2d& pose,
               const ChassisSpeeds& velocity,
               const ChassisAccelerations& acceleration,
               const std::vector<SwerveModuleState>& states)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        states{states} {}

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  template <size_t NumModules>
  SwerveSample(wpi::units::second_t timestamp, const Pose2d& pose,
               const ChassisSpeeds& velocity,
               const ChassisAccelerations& acceleration,
               const SwerveDriveKinematics<NumModules>& kinematics)
      : TrajectorySample{timestamp, pose, velocity, acceleration} {
    auto moduleStates = kinematics.ToSwerveModuleStates(velocity);
    states.assign(moduleStates.begin(), moduleStates.end());
  }

  /**
   * Constructs a SwerveSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param states The swerve module states.
   */
  template <typename T>
  SwerveSample(const TrajectorySample<T>& sample,
               std::initializer_list<SwerveModuleState> states)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        states{states} {}

  /**
   * Constructs a SwerveSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param states The swerve module states.
   */
  template <typename T>
  SwerveSample(const TrajectorySample<T>& sample,
               std::span<const SwerveModuleState> states)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        states{states.begin(), states.end()} {}

  /**
   * Constructs a SwerveSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param states The swerve module states.
   */
  template <typename T>
  SwerveSample(const TrajectorySample<T>& sample,
               const std::vector<SwerveModuleState>& states)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        states{states} {}

  /**
   * Constructs a SwerveSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  template <typename T, size_t NumModules>
  SwerveSample(const TrajectorySample<T>& sample,
               const SwerveDriveKinematics<NumModules>& kinematics)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration} {
    auto moduleStates = kinematics.ToSwerveModuleStates(sample.velocity);
    states.assign(moduleStates.begin(), moduleStates.end());
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  SwerveSample Transform(const Transform2d& transform) const {
    return SwerveSample{timestamp, pose.TransformBy(transform), velocity,
                        acceleration, states};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  SwerveSample RelativeTo(const Pose2d& other) const {
    return SwerveSample{timestamp, pose.RelativeTo(other), velocity,
                        acceleration, states};
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param newTimestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  SwerveSample WithNewTimestamp(wpi::units::second_t newTimestamp) const {
    return SwerveSample{newTimestamp, pose, velocity, acceleration, states};
  }

  /**
   * Checks equality between this SwerveSample and another object.
   */
  bool operator==(const SwerveSample& other) const = default;
};

}  // namespace wpi::math
