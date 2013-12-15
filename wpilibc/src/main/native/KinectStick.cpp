/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "KinectStick.h"

#include "DriverStation.h"
#include "Joystick.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/UsageReporting.h"
#include "Utility.h"
#include "WPIErrors.h"

uint32_t KinectStick::_recentPacketNumber = 0;
KinectStick::KinectStickData KinectStick::_sticks;

#define kJoystickBundleID kFRC_NetworkCommunication_DynamicType_Kinect_Joystick
#define kTriggerMask 1
#define kTopMask 2

/**
 * Kinect joystick constructor
 * @param id value is either 1 or 2 for the left or right joystick decoded from
 * gestures interpreted by the Kinect server on the Driver Station computer.
 */
KinectStick::KinectStick(int id)
{
	if (id != 1 && id != 2)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "KinectStick ID must be 1 or 2");
		return;
	}
	m_id = id;

	nUsageReporting::report(nUsageReporting::kResourceType_KinectStick, id);
}

/**
 * Get the X value of the KinectStick. This axis
 * is unimplemented in the default gestures but can
 * be populated by teams editing the Kinect Server.
 * @param hand Unused
 * @return The X value of the KinectStick
 */
float KinectStick::GetX(JoystickHand hand)
{
	return GetRawAxis(Joystick::kDefaultXAxis);
}

/**
 * Get the Y value of the KinectStick. This axis
 * represents arm angle in the default gestures
 * @param hand Unused
 * @return The Y value of the KinectStick
 */
float KinectStick::GetY(JoystickHand hand)
{
	return GetRawAxis(Joystick::kDefaultYAxis);
}

/**
 * Get the Z value of the KinectStick. This axis
 * is unimplemented in the default gestures but can
 * be populated by teams editing the Kinect Server.
 * @param hand Unused
 * @return The Z value of the KinectStick
 */
float KinectStick::GetZ()
{
	return GetRawAxis(Joystick::kDefaultZAxis);
}

/**
 * Get the Twist value of the KinectStick. This axis
 * is unimplemented in the default gestures but can
 * be populated by teams editing the Kinect Server.
 * @return The Twist value of the KinectStick
 */
float KinectStick::GetTwist()
{
	return GetRawAxis(Joystick::kDefaultTwistAxis);
}

/**
 * Get the Throttle value of the KinectStick. This axis
 * is unimplemented in the default gestures but can
 * be populated by teams editing the Kinect Server.
 * @return The Throttle value of the KinectStick
 */
float KinectStick::GetThrottle()
{
	return GetRawAxis(Joystick::kDefaultThrottleAxis);
}

/**
 * Get the value of the KinectStick axis.
 *
 * @param axis The axis to read [1-6].
 * @return The value of the axis
 */
float KinectStick::GetRawAxis(uint32_t axis)
{
	if (StatusIsFatal()) return 0.0;

	GetData();
	float value = ConvertRawToFloat(_sticks.formatted.rawSticks[m_id - 1].axis[axis-1]);
	return value;
}

/**
 * Get the button value for the button set as the default trigger
 *
 * @param hand Unused
 * @return The state of the button.
 */
bool KinectStick::GetTrigger(JoystickHand hand)
{
	return GetRawButton(kTriggerMask);
}

/**
 * Get the button value for the button set as the default top
 *
 * @param hand Unused
 * @return The state of the button.
 */
bool KinectStick::GetTop(JoystickHand hand)
{
	return GetRawButton(kTopMask);
}

/**
 * Get the button value for the button set as the default bumper (button 4)
 *
 * @param hand Unused
 * @return The state of the button.
 */
bool KinectStick::GetBumper(JoystickHand hand)
{
	// TODO: Should this even be in GenericHID?  Is 4 an appropriate mask value (button 3)?
	return GetRawButton(4);
}

/**
 * Get the button value for buttons 1 through 12. The default gestures
 * implement only 9 buttons.
 *
 * The appropriate button is returned as a boolean value.
 *
 * @param button The button number to be read.
 * @return The state of the button.
 */
bool KinectStick::GetRawButton(uint32_t button)
{
	if (StatusIsFatal()) return false;

	GetData();
	return (_sticks.formatted.rawSticks[m_id - 1].buttons & (1 << button)) != 0;
}

/**
 * Get dynamic data from the driver station buffer
 */
void KinectStick::GetData()
{
	uint32_t packetNumber = DriverStation::GetInstance()->GetPacketNumber();
	if (_recentPacketNumber != packetNumber)
	{
		_recentPacketNumber = packetNumber;
		int retVal = getDynamicControlData(kJoystickBundleID, _sticks.data, sizeof(_sticks.data), 5);
		if (retVal == 0)
		{
			wpi_assert(_sticks.formatted.size == sizeof(_sticks.data) - 1);
		}
	}
}

/**
 * Convert an 8 bit joystick value to a floating point (-1,1) value
 * @param value The 8 bit raw joystick value returned from the driver station
 */
float KinectStick::ConvertRawToFloat(int8_t value)
{
	float result;

	if (value < 0)
		result = ((float) value) / 128.0;
	else
		result = ((float) value) / 127.0;

	wpi_assert(result <= 1.0 && result >= -1.0);

	if (result > 1.0)
		result = 1.0;
	else if (result < -1.0)
		result = -1.0;

	return result;
}
