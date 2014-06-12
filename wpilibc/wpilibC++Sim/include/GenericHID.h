/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef GENERIC_HID_H
#define GENERIC_HID_H

#include <stdint.h>

/** GenericHID Interface
 */
class GenericHID
{
public:
	typedef enum {
		kLeftHand = 0,
		kRightHand = 1
	} JoystickHand;

	virtual ~GenericHID() {}

	virtual float GetX(JoystickHand hand = kRightHand) = 0;
	virtual float GetY(JoystickHand hand = kRightHand) = 0;
	virtual float GetZ() = 0;
	virtual float GetTwist() = 0;
	virtual float GetThrottle() = 0;
	virtual float GetRawAxis(uint32_t axis) = 0;

	virtual bool GetTrigger(JoystickHand hand = kRightHand) = 0;
	virtual bool GetTop(JoystickHand hand = kRightHand) = 0;
	virtual bool GetBumper(JoystickHand hand = kRightHand) = 0;
	virtual bool GetRawButton(uint32_t button) = 0;
};

#endif

