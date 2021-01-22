// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/array.h>

#include "Eigen/Core"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps an Unscented Kalman Filter to fuse latency-compensated
 * vision measurements with mecanum drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an
 * easy but more accurate drop-in for MecanumDriveOdometry.
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 0.02s, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 *
 * Our state-space system is:
 *
 * <strong> x = [[x, y, theta]]^T </strong> in the
 * field-coordinate system.
 *
 * <strong> u = [[vx, vy, omega]]^T </strong> in the field-coordinate system.
 *
 * <strong> y = [[x, y, theta]]^T </strong> in field
 * coords from vision, or <strong> y = [[theta]]^T
 * </strong> from the gyro.
 */
class MecanumDrivePoseEstimator {
 public:
  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param kinematics               A correctly-configured kinematics object
   *                                 for your drivetrain.
   * @param stateStdDevs             Standard deviations of model states.
   *                                 Increase these numbers to trust your
   *                                 model's state estimates less. This matrix
   *                                 is in the form [x, y, theta]^T, with units
   *                                 in meters and radians.
   * @param localMeasurementStdDevs  Standard deviations of the encoder and gyro
   *                                 measurements. Increase these numbers to
   *                                 trust sensor readings from encoders
   *                                 and gyros less. This matrix is in the form
   *                                 [theta], with units in radians.
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]^T, with units in meters and
   *                                 radians.
   * @param nominalDt                The time in seconds between each robot
   *                                 loop.
   */
  MecanumDrivePoseEstimator(
      const Rotation2d& gyroAngle, const Pose2d& initialPose,
      MecanumDriveKinematics kinematics,
      const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 1>& localMeasurementStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs,
      units::second_t nominalDt = 0.02_s);

  /**
   * Sets the pose estimator's trust of global measurements. This might be used
   * to change trust in vision measurements after the autonomous period, or to
   * change trust as distance to a vision target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these numbers to
   *                                 trust global measurements from vision
   *                                 less. This matrix is in the form
   *                                 [x, y, theta]^T, with units in meters and
   *                                 radians.
   */
  void SetVisionMeasurementStdDevs(
      const wpi::array<double, 3>& visionMeasurementStdDevs);

  /**
   * Resets the robot's position on the field.
   *
   * <p>You NEED to reset your encoders (to zero) when calling this method.
   *
   * <p>The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle  The angle reported by the gyroscope.
   */
  void ResetPosition(const Pose2d& pose, const Rotation2d& gyroAngle);

  /**
   * Gets the pose of the robot at the current time as estimated by the Extended
   * Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct
   * the odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
   *
   * @param visionRobotPose The pose of the robot as measured by the vision
   *                        camera.
   * @param timestamp       The timestamp of the vision measurement in seconds.
   *                        Note that if you don't use your own time source by
   *                        calling UpdateWithTime() then you must use a
   *                        timestamp with an epoch since FPGA startup
   *                        (i.e. the epoch of this timestamp is the same
   *                        epoch as Timer#GetFPGATimestamp.) This means
   *                        that you should use Timer#GetFPGATimestamp as your
   *                        time source or sync the epochs.
   */
  void AddVisionMeasurement(const Pose2d& visionRobotPose,
                            units::second_t timestamp);

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param gyroAngle   The current gyro angle.
   * @param wheelSpeeds The current speeds of the mecanum drive wheels.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d Update(const Rotation2d& gyroAngle,
                const MecanumDriveWheelSpeeds& wheelSpeeds);

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder
   * information. This should be called every loop, and the correct loop period
   * must be passed into the constructor of this class.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle          The current gyroscope angle.
   * @param wheelSpeeds        The current speeds of the mecanum drive wheels.
   * @return The estimated pose of the robot in meters.
   */
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        const MecanumDriveWheelSpeeds& wheelSpeeds);

 private:
  UnscentedKalmanFilter<3, 3, 1> m_observer;
  MecanumDriveKinematics m_kinematics;
  KalmanFilterLatencyCompensator<3, 3, 1, UnscentedKalmanFilter<3, 3, 1>>
      m_latencyCompensator;
  std::function<void(const Eigen::Matrix<double, 3, 1>& u,
                     const Eigen::Matrix<double, 3, 1>& y)>
      m_visionCorrect;

  Eigen::Matrix3d m_visionDiscR;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  template <int Dim>
  static wpi::array<double, Dim> StdDevMatrixToArray(
      const Eigen::Matrix<double, Dim, 1>& vector) {
    wpi::array<double, Dim> array;
    for (size_t i = 0; i < Dim; ++i) {
      array[i] = vector(i);
    }
    return array;
  }
};

}  // namespace frc
