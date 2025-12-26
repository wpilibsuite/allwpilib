// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a differential drive trajectory.
 */
class WPILIB_DLLEXPORT DifferentialSample
    : public TrajectorySample {
 public:
  /**
   * The left wheel speed at this sample.
   */
  wpi::units::meters_per_second_t leftSpeed{0_mps};

  /**
   * The right wheel speed at this sample.
   */
  wpi::units::meters_per_second_t rightSpeed{0_mps};

  constexpr DifferentialSample() = default;

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory
   *                  start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference
   *                 frame).
   * @param acceleration The robot acceleration at this sample (in the robot's
   *                     reference frame).
   * @param leftSpeed The left-wheel speed at this sample.
   * @param rightSpeed The right-wheel speed at this sample.
   */
  constexpr DifferentialSample(wpi::units::second_t timestamp,
                               const Pose2d& pose,
                               const ChassisSpeeds& velocity,
                               const ChassisAccelerations& acceleration,
                               wpi::units::meters_per_second_t leftSpeed,
                               wpi::units::meters_per_second_t rightSpeed)
      : TrajectorySample{timestamp, pose, velocity, acceleration},
        leftSpeed{leftSpeed},
        rightSpeed{rightSpeed} {}

  /**
   * Constructs a DifferentialSample.
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
  constexpr DifferentialSample(wpi::units::second_t timestamp,
                               const Pose2d& pose,
                               const ChassisSpeeds& velocity,
                               const ChassisAccelerations& acceleration,
                               const DifferentialDriveKinematics& kinematics)
      : TrajectorySample{timestamp, pose, velocity, acceleration} {
    auto wheelSpeeds = kinematics.ToWheelSpeeds(velocity);
    leftSpeed = wheelSpeeds.left;
    rightSpeed = wheelSpeeds.right;
  }

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param leftSpeed The left-wheel speed at this sample.
   * @param rightSpeed The right-wheel speed at this sample.
   */
  template <typename T>
  constexpr DifferentialSample(const TrajectorySample<T>& sample,
                               wpi::units::meters_per_second_t leftSpeed,
                               wpi::units::meters_per_second_t rightSpeed)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration},
        leftSpeed{leftSpeed},
        rightSpeed{rightSpeed} {}

  /**
   * Constructs a DifferentialSample from a TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  constexpr DifferentialSample(const TrajectorySample& sample,
                               const DifferentialDriveKinematics& kinematics)
      : TrajectorySample{sample.timestamp, sample.pose, sample.velocity,
                         sample.acceleration} {
    auto wheelSpeeds = kinematics.ToWheelSpeeds(sample.velocity);
    leftSpeed = wheelSpeeds.left;
    rightSpeed = wheelSpeeds.right;
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr DifferentialSample Transform(const Transform2d& transform) const {
    return DifferentialSample{timestamp, pose.TransformBy(transform),
                              velocity,  acceleration,
                              leftSpeed, rightSpeed};
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr TrajectorySample RelativeTo(const Pose2d& other) const {
    return DifferentialSample{timestamp, pose.RelativeTo(other),
                              velocity,  acceleration,
                              leftSpeed, rightSpeed};
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param newTimestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr DifferentialSample WithNewTimestamp(
      wpi::units::second_t newTimestamp) const {
    return DifferentialSample{newTimestamp, pose,      velocity,
                              acceleration, leftSpeed, rightSpeed};
  }

  /**
   * Checks equality between this DifferentialSample and another object.
   */
  constexpr bool operator==(const DifferentialSample& other) const = default;
};

}  // namespace wpi::math
