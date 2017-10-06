#ifndef DriveWithJoystick_H
#define DriveWithJoystick_H

#include <Commands/Command.h>

/**
 * This command allows PS3 joystick to drive the robot. It is always running
 * except when interrupted by another command.
 */
class DriveWithJoystick : public frc::Command {
public:
	DriveWithJoystick();
	void Execute() override;
	bool IsFinished() override;
	void End() override;
};

#endif  // DriveWithJoystick_H
