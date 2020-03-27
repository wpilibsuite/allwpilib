/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/units.h>

#include "frc/estimator/KalmanFilter.h"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"

namespace frc {

template <int N>
using Vector = Eigen::Matrix<double, N, 1>;

/**
 * This class wraps an Extended Kalman Filter to fuse latency-compensated vision
 * measurements with differential drive encoder measurements. It will correct
 * for noisy vision measurements and encoder drift. It is intended to be an easy
 * drop-in for DifferentialDriveOdometry. In fact, if you never call
 * AddVisionMeasurement(), and only call Update(), this will behave exactly the
 * same as DifferentialDriveOdometry.
 *
 * Update() should be called every robot loop (if your robot loops are faster or
 * slower than the default, then you should change the nominal delta time via
 * the constructor).
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 *
 * Our state-space system is:
 *
 * x = [[x, y, dtheta]]^T in the field coordinate system.
 *
 * u = [[d_l, d_r, dtheta]]^T -- these aren't technically system inputs, but
 * they make things considerably easier.
 *
 * y = [[x, y, theta]]^T
 */
class DifferentialDrivePoseEstimator {
 public:
  /**
   * Constructs a DifferentialDrivePose estimator.
   *
   * @param gyroAngle          The current gyro angle.
   * @param initialPose        The starting pose estimate.
   * @param stateStdDevs       Standard deviations of model states. Increase
   *                           these numbers to trust your encoders less.
   * @param measurementStdDevs Standard deviations of the measurements. Increase
   *                           these numbers to trust vision less.
   * @param nominalDt          The time in seconds between each robot loop.
   */
  DifferentialDrivePoseEstimator(const Rotation2d& gyroAngle,
                                 const Pose2d& initialPose,
                                 const Vector<3>& stateStdDevs,
                                 const Vector<3>& measurementStdDevs,
                                 units::second_t nominalDt = 0.02_s);

  /**
   * Resets the robot's position on the field.
   *
   * You NEED to reset your encoders to zero when calling this method. The
   * gyroscope angle does not need to be reset here on the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   *@param pose The estimated pose of the robot on the field.
   *@param gyroAngle The current gyro angle.
   */
  void ResetPosition(const Pose2d& pose, const Rotation2d& gyroAngle);

  /**
   * Returns the pose of the robot at the current time as estimated by the
   * Extended Kalman Filter.
   *
   * @return The estimated robot pose.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Adds a vision measurement to the Extended Kalman Filter. This will correct
   * the odometry pose estimate while still accounting for measurement noise.
   *
   * This method can be called as infrequently as you want, as long as you are
   * calling Update() every loop.
   *
   * @param visionRobotPose The pose of the robot as measured by the vision
   *                        camera.
   * @param timestamp       The timestamp of the vision measurement in seconds.
   *                        Note that if you don't use your own time source by
   *                        calling UpdateWithTime(), then you must use a
   *                        timestamp with an epoch since FPGA startup (i.e. the
   *                        epoch of this timestamp is the same epoch as
   *                        frc2::Timer::GetFPGATimestamp(). This means that
   *                        you should use frc2::Timer::GetFPGATimestamp() as
   *                        your time source in this case.
   */
  void AddVisionMeasurement(const Pose2d& visionRobotPose,
                            units::second_t timestamp);

  /**
   * Updates the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop iteration.
   *
   * @param gyroAngle     The current gyro angle.
   * @param leftDistance  The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   *
   * @return The estimated pose of the robot.
   */
  Pose2d Update(const Rotation2d& gyroAngle,
                units::meters_per_second_t leftVelocity,
                units::meters_per_second_t rightVelocity);

  /**
   * Updates the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop iteration.
   *
   * @param currentTime   The time at which this method was called.
   * @param gyroAngle     The current gyro angle.
   * @param leftDistance  The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   *
   * @return The estimated pose of the robot.
   */
  Pose2d UpdateWithTime(units::second_t currentTime,
                        const Rotation2d& gyroAngle,
                        units::meters_per_second_t leftVelocity,
                        units::meters_per_second_t rightVelocity);

 private:
  KalmanFilter<3, 3, 3> m_observer;
  KalmanFilterLatencyCompensator<3, 3, 3, KalmanFilter<3, 3, 3>>
      m_latencyCompensator;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  static LinearSystem<3, 3, 3>& GetObserverSystem();
  static std::array<double, 3> StdDevMatrixToArray(const Vector<3>& stdDevs);
};

}  // namespace frc
