#ifndef TankDriveWithJoystick_H
#define TankDriveWithJoystick_H

#include <Commands/Command.h>

/**
 * Have the robot drive tank style using the PS3 Joystick until interrupted.
 */
class TankDriveWithJoystick: public frc::Command {
public:
	TankDriveWithJoystick();
	void Execute() override;
	bool IsFinished() override;
	void End() override;
};

#endif  // TankDriveWithJoystick_H
