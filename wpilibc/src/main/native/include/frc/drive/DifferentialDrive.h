// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/raw_ostream.h>

#include "frc/drive/RobotDriveBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SpeedController;

/**
 * A class for driving differential drive/skid-steer drive platforms such as
 * the Kit of Parts drive base, "tank drive", or West Coast Drive.
 *
 * These drive bases typically have drop-center / skid-steer with two or more
 * wheels per side (e.g., 6WD or 8WD). This class takes a SpeedController per
 * side. For four and six motor drivetrains, construct and pass in
 * SpeedControllerGroup instances as follows.
 *
 * Four motor drivetrain:
 * @code{.cpp}
 * class Robot {
 *  public:
 *   frc::PWMVictorSPX m_frontLeft{1};
 *   frc::PWMVictorSPX m_rearLeft{2};
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_rearLeft};
 *
 *   frc::PWMVictorSPX m_frontRight{3};
 *   frc::PWMVictorSPX m_rearRight{4};
 *   frc::SpeedControllerGroup m_right{m_frontRight, m_rearRight};
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
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_midLeft, m_rearLeft};
 *
 *   frc::PWMVictorSPX m_frontRight{4};
 *   frc::PWMVictorSPX m_midRight{5};
 *   frc::PWMVictorSPX m_rearRight{6};
 *   frc::SpeedControllerGroup m_right{m_frontRight, m_midRight, m_rearRight};
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
 * Each Drive() function provides different inverse kinematic relations for a
 * differential drive robot. Motor outputs for the right side are negated, so
 * motor direction inversion by the user is usually unnecessary.
 *
 * This library uses the NED axes convention (North-East-Down as external
 * reference in the world frame):
 * http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * The positive X axis points ahead, the positive Y axis points to the right,
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.
 *
 * Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().
 *
 * <p>RobotDrive porting guide:
 * <br>TankDrive(double, double, bool) is equivalent to
 * RobotDrive#TankDrive(double, double, bool) if a deadband of 0 is used.
 * <br>ArcadeDrive(double, double, bool) is equivalent to
 * RobotDrive#ArcadeDrive(double, double, bool) if a deadband of 0 is used
 * and the the rotation input is inverted eg ArcadeDrive(y, -rotation, false)
 * <br>CurvatureDrive(double, double, bool) is similar in concept to
 * RobotDrive#Drive(double, double) with the addition of a quick turn
 * mode. However, it is not designed to give exactly the same response.
 */
class DifferentialDrive : public RobotDriveBase,
                          public Sendable,
                          public SendableHelper<DifferentialDrive> {
 public:
  static constexpr double kDefaultQuickStopThreshold = 0.2;
  static constexpr double kDefaultQuickStopAlpha = 0.1;

  /**
   * Construct a DifferentialDrive.
   *
   * To pass multiple motors per side, use a SpeedControllerGroup. If a motor
   * needs to be inverted, do so before passing it in.
   */
  DifferentialDrive(SpeedController& leftMotor, SpeedController& rightMotor);

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
   *                      [-1.0..1.0]. Clockwise is positive.
   * @param squareInputs If set, decreases the input sensitivity at low speeds.
   */
  void ArcadeDrive(double xSpeed, double zRotation, bool squareInputs = true);

  /**
   * Curvature drive method for differential drive platform.
   *
   * The rotation argument controls the curvature of the robot's path rather
   * than its rate of heading change. This makes the robot more controllable at
   * high speeds. Also handles the robot's quick turn functionality - "quick
   * turn" overrides constant-curvature turning for turn-in-place maneuvers.
   *
   * @param xSpeed      The robot's speed along the X axis [-1.0..1.0]. Forward
   *                    is positive.
   * @param zRotation   The robot's rotation rate around the Z axis [-1.0..1.0].
   *                    Clockwise is positive.
   * @param isQuickTurn If set, overrides constant-curvature turning for
   *                    turn-in-place maneuvers.
   */
  void CurvatureDrive(double xSpeed, double zRotation, bool isQuickTurn);

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
   * Sets the QuickStop speed threshold in curvature drive.
   *
   * QuickStop compensates for the robot's moment of inertia when stopping after
   * a QuickTurn.
   *
   * While QuickTurn is enabled, the QuickStop accumulator takes on the rotation
   * rate value outputted by the low-pass filter when the robot's speed along
   * the X axis is below the threshold. When QuickTurn is disabled, the
   * accumulator's value is applied against the computed angular power request
   * to slow the robot's rotation.
   *
   * @param threshold X speed below which quick stop accumulator will receive
   *                  rotation rate values [0..1.0].
   */
  void SetQuickStopThreshold(double threshold);

  /**
   * Sets the low-pass filter gain for QuickStop in curvature drive.
   *
   * The low-pass filter filters incoming rotation rate commands to smooth out
   * high frequency changes.
   *
   * @param alpha Low-pass filter gain [0.0..2.0]. Smaller values result in
   *              slower output changes. Values between 1.0 and 2.0 result in
   *              output oscillation. Values below 0.0 and above 2.0 are
   *              unstable.
   */
  void SetQuickStopAlpha(double alpha);

  /**
   * Gets if the power sent to the right side of the drivetrain is multiplied by
   * -1.
   *
   * @return true if the right side is inverted
   */
  bool IsRightSideInverted() const;

  /**
   * Sets if the power sent to the right side of the drivetrain should be
   * multiplied by -1.
   *
   * @param rightSideInverted true if right side power should be multiplied by
   * -1
   */
  void SetRightSideInverted(bool rightSideInverted);

  void StopMotor() override;
  void GetDescription(wpi::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  SpeedController* m_leftMotor;
  SpeedController* m_rightMotor;

  double m_quickStopThreshold = kDefaultQuickStopThreshold;
  double m_quickStopAlpha = kDefaultQuickStopAlpha;
  double m_quickStopAccumulator = 0.0;
  double m_rightSideInvertMultiplier = -1.0;
};

}  // namespace frc
