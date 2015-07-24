#ifndef OI_H
#define OI_H

#include "WPILib.h"
#include "Triggers/DoubleButton.h"

class OI {
private:
	Joystick joystick;
	JoystickButton L1, L2, R1, R2;
	DoubleButton sticks;
public:
	OI();
	Joystick* GetJoystick();
};

#endif
