#ifndef DriveWithJoystick_H
#define DriveWithJoystick_H

#include "WPILib.h"

/**
 * This command allows PS3 joystick to drive the robot. It is always running
 * except when interrupted by another command.
 */
class DriveWithJoystick: public Command {
public:
	DriveWithJoystick();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
