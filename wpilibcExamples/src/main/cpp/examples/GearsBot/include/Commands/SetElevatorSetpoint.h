/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * Move the elevator to a given location. This command finishes when it is
 * within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other
 * commands using the elevator should make sure they disable PID!
 */
class SetElevatorSetpoint : public frc::Command {
 public:
  explicit SetElevatorSetpoint(double setpoint);
  void Initialize() override;
  bool IsFinished() override;

 private:
  double m_setpoint;
};
