#ifndef TankDriveWithJoystick_H
#define TankDriveWithJoystick_H

#include "Commands/Command.h"

/**
 * Have the robot drive tank style using the PS3 Joystick until interrupted.
 */
class TankDriveWithJoystick: public Command {
public:
	TankDriveWithJoystick();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif
