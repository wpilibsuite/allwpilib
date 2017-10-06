#ifndef OI_H
#define OI_H

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

#include "Triggers/DoubleButton.h"

class OI {
public:
	OI();
	frc::Joystick* GetJoystick();

private:
	frc::Joystick joystick { 0 };

	frc::JoystickButton L1 { &joystick, 11 };
	frc::JoystickButton L2 { &joystick, 9 };
	frc::JoystickButton R1 { &joystick, 12 };
	frc::JoystickButton R2 { &joystick, 10 };

	DoubleButton sticks { &joystick, 2, 3 };
};

#endif
