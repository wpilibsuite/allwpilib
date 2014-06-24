/*
 * DoubleButton.h
 *
 *  Created on: Jun 24, 2014
 *      Author: alex
 */

#ifndef DOUBLEBUTTON_H_
#define DOUBLEBUTTON_H_

#include "WPILib.h"

class DoubleButton : public Trigger {
private:
	Joystick* joy;
	int button1, button2;

public:
	DoubleButton(Joystick* joy, int button1, int button2);

    bool Get();
};

#endif /* DOUBLEBUTTON_H_ */
