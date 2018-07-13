/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Command.h>

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
class OpenClaw : public frc::Command {
 public:
  OpenClaw();
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};
