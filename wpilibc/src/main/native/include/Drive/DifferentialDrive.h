/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
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
 *   frc::Talon m_frontLeft{1};
 *   frc::Talon m_rearLeft{2};
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_rearLeft};
 *
 *   frc::Talon m_frontRight{3};
 *   frc::Talon m_rearRight{4};
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
 *   frc::Talon m_frontLeft{1};
 *   frc::Talon m_midLeft{2};
 *   frc::Talon m_rearLeft{3};
 *   frc::SpeedControllerGroup m_left{m_frontLeft, m_midLeft, m_rearLeft};
 *
 *   frc::Talon m_frontRight{4};
 *   frc::Talon m_midRight{5};
 *   frc::Talon m_rearRight{6};
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
 */
class DifferentialDrive : public RobotDriveBase {
 public:
  DifferentialDrive(SpeedController& leftMotor, SpeedController& rightMotor);
  virtual ~DifferentialDrive() = default;

  DifferentialDrive(const DifferentialDrive&) = delete;
  DifferentialDrive& operator=(const DifferentialDrive&) = delete;

  void ArcadeDrive(double y, double rotation, bool squaredInputs = true);
  void CurvatureDrive(double y, double rotation, bool isQuickTurn);
  void TankDrive(double left, double right, bool squaredInputs = true);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

 private:
  SpeedController& m_leftMotor;
  SpeedController& m_rightMotor;

  double m_quickStopAccumulator = 0.0;
};

}  // namespace frc
