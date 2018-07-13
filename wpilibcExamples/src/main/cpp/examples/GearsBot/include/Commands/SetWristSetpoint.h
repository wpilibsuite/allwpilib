/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * Move the wrist to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the wrist should make sure they disable PID!
 */
class SetWristSetpoint : public frc::Command {
 public:
  explicit SetWristSetpoint(double setpoint);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
};
