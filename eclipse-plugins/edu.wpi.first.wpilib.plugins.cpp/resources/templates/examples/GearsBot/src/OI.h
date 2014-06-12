/*
 * OI.h
 *
 *  Created on: Jun 3, 2014
 *      Author: alex
 */

#ifndef OI_H_
#define OI_H_

#include "WPILib.h"

class OI {
public:
	OI();
	Joystick* GetJoystick();

private:
	Joystick* joy;
};

#endif /* OI_H_ */
