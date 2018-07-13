/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * Opens the claw
 */
class OpenClaw : public frc::Command {
 public:
  OpenClaw();
  void Initialize() override;
  bool IsFinished() override;
};
