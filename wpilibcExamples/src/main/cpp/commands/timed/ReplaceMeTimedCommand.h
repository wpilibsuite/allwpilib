/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Commands/TimedCommand.h"

class ReplaceMeTimedCommand : public frc::TimedCommand {
 public:
  explicit ReplaceMeTimedCommand(double timeout);
  void Initialize() override;
  void Execute() override;
  void End() override;
  void Interrupted() override;
};
