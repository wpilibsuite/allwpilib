// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/apriltag/AprilTagFields.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/estimator/DifferentialDrivePoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/simulation/DifferentialDrivetrainSim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/smartdashboard/Field2d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

/**
 * Represents a differential drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain();

  static constexpr wpi::units::meters_per_second_t kMaxVelocity =
      3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularVelocity{
      std::numbers::pi};  // 1/2 rotation per second

  /**
   * Sets the desired wheel velocities.
   *
   * @param velocities The desired wheel velocities.
   */
  void SetVelocities(
      const wpi::math::DifferentialDriveWheelVelocities& velocities);

  /** Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xVelocity Linear velocity.
   * @param rot Angular Velocity.
   */
  void Drive(wpi::units::meters_per_second_t xVelocity,
             wpi::units::radians_per_second_t rot);

  /**
   * Updates the field-relative position.
   */
  void UpdateOdometry();

  /**
   * This function is called periodically during simulation. */
  void SimulationPeriodic();

  /** This function is called periodically, regardless of mode. */
  void Periodic();

  /**
   * Computes and publishes to a networktables topic the transformation from
   * the camera's pose to the object's pose. This function exists solely for the
   * purposes of simulation, and this would normally be handled by computer
   * vision.
   *
   * <p>The object could be a target or a fiducial marker.
   *
   * @param objectInField The object's field-relative position.
   * @param robotToCamera The transformation from the robot's pose to the
   * camera's pose.
   * @param cameraToObjectEntry The networktables entry publishing and querying
   * example computer vision measurements.
   * @param drivetrainSimulation A DifferentialDrivetrainSim modeling the
   * robot's drivetrain.
   */
  void PublishCameraToObject(
      wpi::math::Pose3d objectInField, wpi::math::Transform3d robotToCamera,
      wpi::nt::DoubleArrayEntry& cameraToObjectEntry,
      wpi::sim::DifferentialDrivetrainSim drivetrainSimulator);

  /**
   * Queries the camera-to-object transformation from networktables to compute
   * the robot's field-relative pose from vision measurements.
   *
   * <p>The object could be a target or a fiducial marker.
   *
   * @param objectInField The object's field-relative position.
   * @param robotToCamera The transformation from the robot's pose to the
   * camera's pose.
   * @param cameraToObjectEntry The networktables entry publishing and querying
   * example computer vision measurements.
   */
  wpi::math::Pose3d ObjectToRobotPose(
      wpi::math::Pose3d objectInField, wpi::math::Transform3d robotToCamera,
      wpi::nt::DoubleArrayEntry& cameraToObjectEntry);

 private:
  static constexpr wpi::units::meter_t kTrackwidth = 0.381_m * 2;
  static constexpr wpi::units::meter_t kWheelRadius = 0.0508_m;
  static constexpr int kEncoderResolution = 4096;

  static constexpr std::array<double, 7> kDefaultVal{0.0, 0.0, 0.0, 0.0,
                                                     0.0, 0.0, 0.0};

  wpi::math::Transform3d robotToCamera{
      wpi::math::Translation3d{1_m, 1_m, 1_m},
      wpi::math::Rotation3d{0_rad, 0_rad,
                            wpi::units::radian_t{std::numbers::pi / 2}}};

  wpi::nt::NetworkTableInstance inst{
      wpi::nt::NetworkTableInstance::GetDefault()};
  wpi::nt::DoubleArrayTopic cameraToObjectTopic{
      inst.GetDoubleArrayTopic("cameraToObjectTopic")};
  wpi::nt::DoubleArrayEntry cameraToObjectEntry =
      cameraToObjectTopic.GetEntry(kDefaultVal);
  wpi::nt::DoubleArrayEntry& cameraToObjectEntryRef = cameraToObjectEntry;

  wpi::apriltag::AprilTagFieldLayout aprilTagFieldLayout{
      wpi::apriltag::AprilTagFieldLayout::LoadField(
          wpi::apriltag::AprilTagField::k2024Crescendo)};
  wpi::math::Pose3d objectInField{aprilTagFieldLayout.GetTagPose(0).value()};

  wpi::PWMSparkMax leftLeader{1};
  wpi::PWMSparkMax leftFollower{2};
  wpi::PWMSparkMax rightLeader{3};
  wpi::PWMSparkMax rightFollower{4};

  wpi::Encoder leftEncoder{0, 1};
  wpi::Encoder rightEncoder{2, 3};

  wpi::math::PIDController leftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController rightPIDController{1.0, 0.0, 0.0};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::DifferentialDriveKinematics kinematics{kTrackwidth};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::DifferentialDrivePoseEstimator poseEstimator{
      kinematics,
      imu.GetRotation2d(),
      wpi::units::meter_t{leftEncoder.GetDistance()},
      wpi::units::meter_t{rightEncoder.GetDistance()},
      wpi::math::Pose2d{},
      {0.01, 0.01, 0.01},
      {0.1, 0.1, 0.1}};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> feedforward{
      1_V, 3_V / 1_mps};

  // Simulation classes
  wpi::sim::EncoderSim leftEncoderSim{leftEncoder};
  wpi::sim::EncoderSim rightEncoderSim{rightEncoder};
  wpi::Field2d fieldSim;
  wpi::Field2d fieldApproximation;
  wpi::math::LinearSystem<2, 2, 2> drivetrainSystem =
      wpi::math::Models::DifferentialDriveFromSysId(
          1.98_V / 1_mps, 0.2_V / 1_mps_sq, 1.5_V / 1_mps, 0.3_V / 1_mps_sq);
  wpi::sim::DifferentialDrivetrainSim drivetrainSimulator{
      drivetrainSystem, kTrackwidth, wpi::math::DCMotor::CIM(2), 8, 2_in};
};
