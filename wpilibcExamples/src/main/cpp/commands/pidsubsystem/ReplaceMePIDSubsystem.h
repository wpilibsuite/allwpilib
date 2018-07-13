/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/PIDSubsystem.h>

class ReplaceMePIDSubsystem : public frc::PIDSubsystem {
 public:
  ReplaceMePIDSubsystem();
  double ReturnPIDInput() override;
  void UsePIDOutput(double output) override;
  void InitDefaultCommand() override;
};
