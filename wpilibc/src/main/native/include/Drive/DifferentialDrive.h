/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <llvm/raw_ostream.h>

#include "Drive/RobotDriveBase.h"

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
 *   frc::Spark m_frontLeft{1};
 *   frc::Spark m_rearLeft{2};
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_rearLeft};
 *
 *   frc::Spark m_frontRight{3};
 *   frc::Spark m_rearRight{4};
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
 *   frc::Spark m_frontLeft{1};
 *   frc::Spark m_midLeft{2};
 *   frc::Spark m_rearLeft{3};
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_midLeft, m_rearLeft};
 *
 *   frc::Spark m_frontRight{4};
 *   frc::Spark m_midRight{5};
 *   frc::Spark m_rearRight{6};
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
class DifferentialDrive : public RobotDriveBase {
 public:
  static constexpr double kDefaultQuickStopThreshold = 0.2;
  static constexpr double kDefaultQuickStopAlpha = 0.1;

  DifferentialDrive(SpeedController& leftMotor, SpeedController& rightMotor);
  ~DifferentialDrive() override = default;

  DifferentialDrive(const DifferentialDrive&) = delete;
  DifferentialDrive& operator=(const DifferentialDrive&) = delete;

  void ArcadeDrive(double xSpeed, double zRotation, bool squaredInputs = true);
  void CurvatureDrive(double xSpeed, double zRotation, bool isQuickTurn);
  void TankDrive(double leftSpeed, double rightSpeed,
                 bool squaredInputs = true);

  void SetQuickStopThreshold(double threshold);
  void SetQuickStopAlpha(double alpha);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  SpeedController& m_leftMotor;
  SpeedController& m_rightMotor;

  double m_quickStopThreshold = kDefaultQuickStopThreshold;
  double m_quickStopAlpha = kDefaultQuickStopAlpha;
  double m_quickStopAccumulator = 0.0;
};

}  // namespace frc
