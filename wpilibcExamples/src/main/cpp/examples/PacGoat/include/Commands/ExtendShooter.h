/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/TimedCommand.h>

/**
 * Extend the shooter and then retract it after a second.
 */
class ExtendShooter : public frc::TimedCommand {
 public:
  ExtendShooter();
  void Initialize() override;
  void End() override;
};
