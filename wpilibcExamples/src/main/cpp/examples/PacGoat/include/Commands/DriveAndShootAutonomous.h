/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/CommandGroup.h>

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
class DriveAndShootAutonomous : public frc::CommandGroup {
 public:
  DriveAndShootAutonomous();
};
