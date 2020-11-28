/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include "Eigen/Core"
#include "frc/estimator/KalmanFilterLatencyCompensator.h"
#include "frc/estimator/UnscentedKalmanFilter.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"
#include "units/time.h"

namespace frc {
/**
 * This class wraps an Unscented Kalman Filter to fuse latency-compensated
 * vision measurements with differential drive encoder measurements. It will
 * correct for noisy vision measurements and encoder drift. It is intended to be
 * an easy drop-in for DifferentialDriveOdometry. In fact, if you never call
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
 * <strong> x = [[x, y, theta, dist_l, dist_r]]^T </strong> in the field
 * coordinate system.
 *
 * <strong> u = [[d_l, d_r, dtheta]]^T </strong> (robot-relative velocities) --
 * NB: using velocities make things considerably easier, because it means that
 * teams don't have to worry about getting an accurate model. Basically, we
 * suspect that it's easier for teams to get good encoder data than it is for
 * them to perform system identification well enough to get a good model
 *
 * <strong>y = [[x, y, theta]]^T </strong> from vision,
 * or <strong>y = [[dist_l, dist_r, theta]] </strong> from encoders and gyro.
 */
class DifferentialDrivePoseEstimator {
 public:
  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param gyroAngle                The gyro angle of the robot.
   * @param initialPose              The estimated initial pose.
   * @param stateStdDevs             Standard deviations of the model states.
   *                                 Increase these to trust your wheel speeds
   *                                 less.
   * @param localMeasurementStdDevs  Standard deviations of the encoder
   *                                 measurements. Increase these to trust
   *                                 encoder distances less.
   * @param visionMeasurementStdDevs Standard deviations of the vision
   *                                 measurements. Increase these to trust
   *                                 vision less.
   * @param nominalDt                The period of the loop calling Update().
   */
  DifferentialDrivePoseEstimator(
      const Rotation2d& gyroAngle, const Pose2d& initialPose,
      const std::array<double, 5>& stateStdDevs,
      const std::array<double, 3>& localMeasurementStdDevs,
      const std::array<double, 3>& visionMeasurementStdDevs,
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
   * Unscented Kalman Filter.
   *
   * @return The estimated robot pose.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Adds a vision measurement to the Unscented Kalman Filter. This will correct
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
   * Updates the Unscented Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop iteration.
   *
   * @param gyroAngle     The current gyro angle.
   * @param leftDistance  The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   *
   * @return The estimated pose of the robot.
   */
  Pose2d Update(const Rotation2d& gyroAngle,
                const DifferentialDriveWheelSpeeds& wheelSpeeds,
                units::meter_t leftDistance, units::meter_t rightDistance);

  /**
   * Updates the Unscented Kalman Filter using only wheel encoder information.
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
                        const DifferentialDriveWheelSpeeds& wheelSpeeds,
                        units::meter_t leftDistance,
                        units::meter_t rightDistance);

 private:
  UnscentedKalmanFilter<5, 3, 3> m_observer;
  KalmanFilterLatencyCompensator<5, 3, 3, UnscentedKalmanFilter<5, 3, 3>>
      m_latencyCompensator;
  std::function<void(const Eigen::Matrix<double, 3, 1>& u,
                     const Eigen::Matrix<double, 3, 1>& y)>
      m_visionCorrect;

  Eigen::Matrix<double, 3, 3> m_visionDiscR;

  units::second_t m_nominalDt;
  units::second_t m_prevTime = -1_s;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  template <int Dim>
  static std::array<double, Dim> StdDevMatrixToArray(
      const Eigen::Matrix<double, Dim, 1>& stdDevs);

  static Eigen::Matrix<double, 5, 1> F(const Eigen::Matrix<double, 5, 1>& x,
                                       const Eigen::Matrix<double, 3, 1>& u);
  static Eigen::Matrix<double, 5, 1> FillStateVector(
      const Pose2d& pose, units::meter_t leftDistance,
      units::meter_t rightDistance);
};

}  // namespace frc
