#ifndef OI_H_
#define OI_H_

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

class OI {
public:
	OI();
	frc::Joystick* GetJoystick();

private:
	frc::Joystick joy { 0 };

	// Create some buttons
	frc::JoystickButton d_up { &joy, 5 };
	frc::JoystickButton d_right { &joy, 6 };
	frc::JoystickButton d_down { &joy, 7 };
	frc::JoystickButton d_left { &joy, 8 };
	frc::JoystickButton l2 { &joy, 9 };
	frc::JoystickButton r2 { &joy, 10 };
	frc::JoystickButton l1 { &joy, 11 };
	frc::JoystickButton r1 { &joy, 12 };
};

#endif  // OI_H_
