#include "DoubleButton.h"

#include <Joystick.h>

DoubleButton::DoubleButton(frc::Joystick* joy, int button1, int button2) {
	this->joy = joy;
	this->button1 = button1;
	this->button2 = button2;
}

bool DoubleButton::Get() {
	return joy->GetRawButton(button1) && joy->GetRawButton(button2);
}

