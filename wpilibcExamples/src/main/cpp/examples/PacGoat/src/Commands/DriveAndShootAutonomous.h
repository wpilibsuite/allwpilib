#ifndef DriveAndShootAutonomous_H
#define DriveAndShootAutonomous_H

#include <Commands/CommandGroup.h>

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
class DriveAndShootAutonomous: public frc::CommandGroup {
public:
	DriveAndShootAutonomous();
};

#endif  // DriveAndShootAutonomous_H
