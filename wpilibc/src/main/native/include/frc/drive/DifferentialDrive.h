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
 * A class for driving differential drive/skid-steer drive platforms such as
 * the Kit of Parts drive base, "tank drive", or West Coast Drive.
 *
 * These drive bases typically have drop-center / skid-steer with two or more
 * wheels per side (e.g., 6WD or 8WD). This class takes a MotorController per
 * side. For four and six motor drivetrains, construct and pass in
 * MotorControllerGroup instances as follows.
 *
 * Four motor drivetrain:
 * @code{.cpp}
 * class Robot {
 *  public:
 *   frc::PWMVictorSPX m_frontLeft{1};
 *   frc::PWMVictorSPX m_rearLeft{2};
 *   frc::MotorControllerGroup m_left{m_frontLeft, m_rearLeft};
 *
 *   frc::PWMVictorSPX m_frontRight{3};
 *   frc::PWMVictorSPX m_rearRight{4};
 *   frc::MotorControllerGroup m_right{m_frontRight, m_rearRight};
 *
 *   frc::DifferentialDrive m_drive{m_left, m_right};
 * };
 * @endcode
 *
 * Six motor drivetrain:
 * @code{.cpp}
 * class Robot {
 *  public:
 *   frc::PWMVictorSPX m_frontLeft{1};
 *   frc::PWMVictorSPX m_midLeft{2};
 *   frc::PWMVictorSPX m_rearLeft{3};
 *   frc::MotorControllerGroup m_left{m_frontLeft, m_midLeft, m_rearLeft};
 *
 *   frc::PWMVictorSPX m_frontRight{4};
 *   frc::PWMVictorSPX m_midRight{5};
 *   frc::PWMVictorSPX m_rearRight{6};
 *   frc::MotorControllerGroup m_right{m_frontRight, m_midRight, m_rearRight};
 *
 *   frc::DifferentialDrive m_drive{m_left, m_right};
 * };
 * @endcode
 *
 * A differential drive robot has left and right wheels separated by an
 * arbitrary width.
 *
 * Drive base diagram:
 * <pre>
 * |_______|
 * | |   | |
 *   |   |
 * |_|___|_|
 * |       |
 * </pre>
 *
 * Each drive function provides different inverse kinematic relations for a
 * differential drive robot.
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
 * MotorSafety is enabled by default. The tankDrive, arcadeDrive,
 * or curvatureDrive methods should be called periodically to avoid Motor
 * Safety timeouts.
 */
class DifferentialDrive : public RobotDriveBase,
                          public wpi::Sendable,
                          public wpi::SendableHelper<DifferentialDrive> {
 public:
  /**
   * Wheel speeds for a differential drive.
   *
   * Uses normalized voltage [-1.0..1.0].
   */
  struct WheelSpeeds {
    double left = 0.0;
    double right = 0.0;
  };

  /**
   * Construct a DifferentialDrive.
   *
   * To pass multiple motors per side, use a MotorControllerGroup. If a motor
   * needs to be inverted, do so before passing it in.
   */
  DifferentialDrive(MotorController& leftMotor, MotorController& rightMotor);

  ~DifferentialDrive() override = default;

  DifferentialDrive(DifferentialDrive&&) = default;
  DifferentialDrive& operator=(DifferentialDrive&&) = default;

  /**
   * Arcade drive method for differential drive platform.
   *
   * Note: Some drivers may prefer inverted rotation controls. This can be done
   * by negating the value passed for rotation.
   *
   * @param xSpeed        The speed at which the robot should drive along the X
   *                      axis [-1.0..1.0]. Forward is positive.
   * @param zRotation     The rotation rate of the robot around the Z axis
   *                      [-1.0..1.0]. Counterclockwise is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  void ArcadeDrive(double xSpeed, double zRotation, bool squareInputs = true);

  /**
   * Curvature drive method for differential drive platform.
   *
   * The rotation argument controls the curvature of the robot's path rather
   * than its rate of heading change. This makes the robot more controllable at
   * high speeds.
   *
   * @param xSpeed           The robot's speed along the X axis [-1.0..1.0].
   *                         Forward is positive.
   * @param zRotation        The normalized curvature [-1.0..1.0].
   *                         Counterclockwise is positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for
   *                         turn-in-place maneuvers. zRotation will control
   *                         turning rate instead of curvature.
   */
  void CurvatureDrive(double xSpeed, double zRotation, bool allowTurnInPlace);

  /**
   * Tank drive method for differential drive platform.
   *
   * @param leftSpeed     The robot left side's speed along the X axis
   *                      [-1.0..1.0]. Forward is positive.
   * @param rightSpeed    The robot right side's speed along the X axis
   *                      [-1.0..1.0]. Forward is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  void TankDrive(double leftSpeed, double rightSpeed, bool squareInputs = true);

  /**
   * Arcade drive inverse kinematics for differential drive platform.
   *
   * Note: Some drivers may prefer inverted rotation controls. This can be done
   * by negating the value passed for rotation.
   *
   * @param xSpeed       The speed at which the robot should drive along the X
   *                     axis [-1.0..1.0]. Forward is positive.
   * @param zRotation    The rotation rate of the robot around the Z axis
   *                     [-1.0..1.0]. Clockwise is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds ArcadeDriveIK(double xSpeed, double zRotation,
                                   bool squareInputs = true);

  /**
   * Curvature drive inverse kinematics for differential drive platform.
   *
   * The rotation argument controls the curvature of the robot's path rather
   * than its rate of heading change. This makes the robot more controllable at
   * high speeds.
   *
   * @param xSpeed           The robot's speed along the X axis [-1.0..1.0].
   *                         Forward is positive.
   * @param zRotation        The normalized curvature [-1.0..1.0]. Clockwise is
   *                         positive.
   * @param allowTurnInPlace If set, overrides constant-curvature turning for
   *                         turn-in-place maneuvers. zRotation will control
   *                         turning rate instead of curvature.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds CurvatureDriveIK(double xSpeed, double zRotation,
                                      bool allowTurnInPlace);

  /**
   * Tank drive inverse kinematics for differential drive platform.
   *
   * @param leftSpeed    The robot left side's speed along the X axis
   *                     [-1.0..1.0]. Forward is positive.
   * @param rightSpeed   The robot right side's speed along the X axis
   *                     [-1.0..1.0]. Forward is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds TankDriveIK(double leftSpeed, double rightSpeed,
                                 bool squareInputs = true);

  void StopMotor() override;
  std::string GetDescription() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  MotorController* m_leftMotor;
  MotorController* m_rightMotor;
};

}  // namespace frc
