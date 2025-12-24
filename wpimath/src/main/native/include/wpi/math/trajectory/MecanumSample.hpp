// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a mecanum drive trajectory.
 */
class WPILIB_DLLEXPORT MecanumSample : public TrajectorySample<MecanumSample> {
 public:
  using seconds_t = wpi::units::second_t;

  /**
   * The speeds of the wheels in the robot's reference frame.
   */
  MecanumDriveWheelSpeeds speeds;

  /**
   * Constructs a MecanumSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param speeds The mecanum wheel speeds.
   */
  constexpr MecanumSample(seconds_t timestamp, const Pose2d& pose,
                          const ChassisSpeeds& velocity,
                          const ChassisAccelerations& acceleration,
                          const MecanumDriveWheelSpeeds& speeds)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        speeds{speeds} {}

  /**
   * Constructs a MecanumSample.
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
  MecanumSample(seconds_t timestamp, const Pose2d& pose,
                const ChassisSpeeds& velocity,
                const ChassisAccelerations& acceleration,
                const MecanumDriveKinematics& kinematics)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        speeds{kinematics.ToWheelSpeeds(velocity)} {}

  /**
   * Constructs a MecanumSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param speeds The mecanum wheel speeds.
   */
  template <typename T>
  constexpr MecanumSample(const TrajectorySample<T>& sample,
                          const MecanumDriveWheelSpeeds& speeds)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        speeds{speeds} {}

  /**
   * Constructs a MecanumSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The mecanum drivetrain kinematics.
   */
  template <typename T>
  MecanumSample(const TrajectorySample<T>& sample,
                const MecanumDriveKinematics& kinematics)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        speeds{kinematics.ToWheelSpeeds(sample.velocity)} {}

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr MecanumSample Transform(const Transform2d& transform) const {
    return MecanumSample{timestamp, pose.TransformBy(transform), velocity,
                         acceleration, speeds};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr MecanumSample RelativeTo(const Pose2d& other) const {
    return MecanumSample{timestamp, pose.RelativeTo(other), velocity,
                         acceleration, speeds};
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param newTimestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr MecanumSample WithNewTimestamp(seconds_t newTimestamp) const {
    return MecanumSample{newTimestamp, pose, velocity, acceleration, speeds};
  }

  /**
   * Checks equality between this MecanumSample and another object.
   */
  constexpr bool operator==(const MecanumSample& other) const = default;
};

}  // namespace wpi::math
