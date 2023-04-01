// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/drive/RobotDriveBase.h"

namespace frc {

class MotorController;

/**
 * A class for driving H drive platforms
 *
 * @endcode
 *
 * Six motor drivetrain:
 * @code{.cpp}
 * class Robot {
 *  public:
 *   frc::PWMVictorSPX m_frontLeft{1};
 *   frc::PWMVictorSPX m_midLeft{2};
 *   frc::MotorControllerGroup m_left{m_frontLeft, m_backLeft};
 *
 *   frc::PWMVictorSPX m_midRight{3};
 *   frc::PWMVictorSPX m_rearRight{4};
 *   frc::MotorControllerGroup m_right{m_frontRight, m_backRight};

 *   frc::PWMVictorSPX m_lateral1{5};
 *   frc::PWMVictorSPX m_lateral2{6};
 *   frc::MotorControllerGroup m_right{m_lateral1, m_lateral2};
 *
 *   frc::HDrive m_drive{m_left, m_right, m_lateral};
 * };
 * @endcode
 *
 * A H drive robot has left, right & lateral wheels separated by an
 * arbitrary width.
 *
 * Drive base diagram:
 * <pre>
 * |_______|
 * | |   | |
 *   |---|
 * |_|___|_|
 * |       |
 * </pre>
 *
 * Each drive function provides different inverse kinematic relations for a
 * H drive robot.
 *
 * This library uses the NWU axes convention (North-West-Up as external
 * reference in the world frame). The positive X axis points ahead, the positive
 * Y axis points to the left, and the positive Z axis points up. Rotations
 * follow the right-hand rule, so counterclockwise rotation around the Z axis is
 * positive.
 *
 * Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().
 *
 * MotorSafety is enabled by default. The tankDrive or arcadeDrive,
 * methods should be called periodically to avoid Motor
 * Safety timeouts.
 */
class HDrive : public RobotDriveBase,
                          public wpi::Sendable,
                          public wpi::SendableHelper<HDrive> {
 public:
  /**
   * Wheel speeds for a differential drive.
   *
   * Uses normalized voltage [-1.0..1.0].
   */
  struct WheelSpeeds {
    double left = 0.0;
    double right = 0.0;
    double lateral = 0.0;
  };

  /**
   * Construct a DifferentialDrive.
   *
   * To pass multiple motors per side, use a MotorControllerGroup. If a motor
   * needs to be inverted, do so before passing it in.
   */
  HDrive(MotorController& leftMotor, MotorController& rightMotor, MotorController& lateralMotor);

  ~HDrive() override = default;

  HDrive(HDrive&&) = default;
  HDrive& operator=(HDrive&&) = default;

  /**
   * Arcade drive method for H drive platform.
   *
   * Note: Some drivers may prefer inverted rotation controls. This can be done
   * by negating the value passed for rotation.
   *
   * @param xSpeed        The speed at which the robot should drive along the X
   *                      axis [-1.0..1.0]. Forward is positive.
   * @param zRotation     The rotation rate of the robot around the Z axis
   *                      [-1.0..1.0]. Counterclockwise is positive.
   * @param ySpeed        The speed at which the robot should drive along the Y
   *                      axis [-1.0..1.0]. Right is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  void ArcadeDrive(double xSpeed, double zRotation, double ySpeed, bool squareInputs = true);

  /**
   * Tank drive method for H drive platform.
   *
   * @param leftSpeed     The robot left side's speed along the X axis
   *                      [-1.0..1.0]. Forward is positive.
   * @param rightSpeed    The robot right side's speed along the X axis
   *                      [-1.0..1.0]. Forward is positive.
   * @param lateralSpeed  The robot speed along the Y axis
   *                      [-1.0..1.0]. Right is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  void TankDrive(double leftSpeed, double rightSpeed, double lateralSpeed, bool squareInputs = true);

  /**
   * Arcade drive inverse kinematics for H drive platform.
   *
   * Note: Some drivers may prefer inverted rotation controls. This can be done
   * by negating the value passed for rotation.
   *
   * @param xSpeed       The speed at which the robot should drive along the X
   *                     axis [-1.0..1.0]. Forward is positive.
   * @param zRotation    The rotation rate of the robot around the Z axis
   *                     [-1.0..1.0]. Clockwise is positive.
   * @param ySpeed       The speed at which the robot should drive along the y
   *                     axis [-1.0..1.0]. Right is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds ArcadeDriveIK(double xSpeed, double zRotation, double ySpeed,
                                   bool squareInputs = true);

  /**
   * Tank drive inverse kinematics for H drive platform.
   *
   * @param leftSpeed    The robot left side's speed along the X axis
   *                     [-1.0..1.0]. Forward is positive.
   * @param rightSpeed   The robot right side's speed along the X axis
   *                     [-1.0..1.0]. Forward is positive.
   * @param lateralSpeed The speed at which the robot should drive along the Y
   *                     axis [-1.0..1.0]. Right is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds TankDriveIK(double leftSpeed, double rightSpeed, double lateralSpeed,
                                 bool squareInputs = true);

  void StopMotor() override;
  std::string GetDescription() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  MotorController* m_leftMotor;
  MotorController* m_rightMotor;
  MotorController* m_lateralMotor;
};

}  // namespace frc