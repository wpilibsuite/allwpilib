#ifndef DOUBLEBUTTON_H_
#define DOUBLEBUTTON_H_

#include <Buttons/Trigger.h>

namespace frc {
class Joystick;
}

class DoubleButton: public frc::Trigger {
public:
	DoubleButton(frc::Joystick* joy, int button1, int button2);

	bool Get();

private:
	frc::Joystick* joy;
	int button1;
	int button2;
};

#endif  // DOUBLEBUTTON_H_
