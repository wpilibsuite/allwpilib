// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/math/util/ComputerVisionUtil.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/apriltag/AprilTagFields.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/estimator/DifferentialDrivePoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Quaternion.hpp"
#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/simulation/DifferentialDrivetrainSim.hpp"
#include "wpi/simulation/EncoderSim.hpp"
#include "wpi/smartdashboard/Field2d.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
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

  static constexpr units::meters_per_second_t kMaxSpeed =
      3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

  /**
   * Sets the desired wheel speeds.
   *
   * @param speeds The desired wheel speeds.
   */
  void SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds);

  /** Drives the robot with the given linear velocity and angular velocity.
   *
   * @param xSpeed Linear velocity.
   * @param rot Angular Velocity.
   */
  void Drive(units::meters_per_second_t xSpeed,
             units::radians_per_second_t rot);

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
      frc::Pose3d objectInField, frc::Transform3d robotToCamera,
      nt::DoubleArrayEntry& cameraToObjectEntry,
      frc::sim::DifferentialDrivetrainSim drivetrainSimulator);

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
  frc::Pose3d ObjectToRobotPose(frc::Pose3d objectInField,
                                frc::Transform3d robotToCamera,
                                nt::DoubleArrayEntry& cameraToObjectEntry);

 private:
  static constexpr units::meter_t kTrackwidth = 0.381_m * 2;
  static constexpr units::meter_t kWheelRadius = 0.0508_m;
  static constexpr int kEncoderResolution = 4096;

  static constexpr std::array<double, 7> kDefaultVal{0.0, 0.0, 0.0, 0.0,
                                                     0.0, 0.0, 0.0};

  frc::Transform3d m_robotToCamera{
      frc::Translation3d{1_m, 1_m, 1_m},
      frc::Rotation3d{0_rad, 0_rad, units::radian_t{std::numbers::pi / 2}}};

  nt::NetworkTableInstance m_inst{nt::NetworkTableInstance::GetDefault()};
  nt::DoubleArrayTopic m_cameraToObjectTopic{
      m_inst.GetDoubleArrayTopic("m_cameraToObjectTopic")};
  nt::DoubleArrayEntry m_cameraToObjectEntry =
      m_cameraToObjectTopic.GetEntry(kDefaultVal);
  nt::DoubleArrayEntry& m_cameraToObjectEntryRef = m_cameraToObjectEntry;

  frc::AprilTagFieldLayout m_aprilTagFieldLayout{
      frc::AprilTagFieldLayout::LoadField(frc::AprilTagField::k2024Crescendo)};
  frc::Pose3d m_objectInField{m_aprilTagFieldLayout.GetTagPose(0).value()};

  frc::PWMSparkMax m_leftLeader{1};
  frc::PWMSparkMax m_leftFollower{2};
  frc::PWMSparkMax m_rightLeader{3};
  frc::PWMSparkMax m_rightFollower{4};

  frc::Encoder m_leftEncoder{0, 1};
  frc::Encoder m_rightEncoder{2, 3};

  frc::PIDController m_leftPIDController{1.0, 0.0, 0.0};
  frc::PIDController m_rightPIDController{1.0, 0.0, 0.0};

  frc::OnboardIMU m_imu{frc::OnboardIMU::kFlat};

  frc::DifferentialDriveKinematics m_kinematics{kTrackwidth};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::DifferentialDrivePoseEstimator m_poseEstimator{
      m_kinematics,
      m_imu.GetRotation2d(),
      units::meter_t{m_leftEncoder.GetDistance()},
      units::meter_t{m_rightEncoder.GetDistance()},
      frc::Pose2d{},
      {0.01, 0.01, 0.01},
      {0.1, 0.1, 0.1}};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SimpleMotorFeedforward<units::meters> m_feedforward{1_V, 3_V / 1_mps};

  // Simulation classes
  frc::sim::EncoderSim m_leftEncoderSim{m_leftEncoder};
  frc::sim::EncoderSim m_rightEncoderSim{m_rightEncoder};
  frc::Field2d m_fieldSim;
  frc::Field2d m_fieldApproximation;
  frc::LinearSystem<2, 2, 2> m_drivetrainSystem =
      frc::LinearSystemId::IdentifyDrivetrainSystem(
          1.98_V / 1_mps, 0.2_V / 1_mps_sq, 1.5_V / 1_mps, 0.3_V / 1_mps_sq);
  frc::sim::DifferentialDrivetrainSim m_drivetrainSimulator{
      m_drivetrainSystem, kTrackwidth, frc::DCMotor::CIM(2), 8, 2_in};
};
