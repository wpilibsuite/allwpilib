/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * This command drives the robot over a given distance with simple proportional
 * control This command will drive a given distance limiting to a maximum speed.
 */
class DriveForward : public frc::Command {
 public:
  DriveForward();
  explicit DriveForward(double dist);
  DriveForward(double dist, double maxSpeed);
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  double m_driveForwardSpeed;
  double m_distance;
  double m_error = 0;
  static constexpr double kTolerance = 0.1;
  static constexpr double kP = -1.0 / 5.0;

  void init(double dist, double maxSpeed);
};
