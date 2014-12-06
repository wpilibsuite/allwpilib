/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <cstdint>
#include "GenericHID.h"
#include "ErrorBase.h"

class DriverStation;

/**
 * Handle input from standard Joysticks connected to the Driver Station.
 * This class handles standard input that comes from the Driver Station. Each time a value is requested
 * the most recent value is returned. There is a single class instance for each joystick and the mapping
 * of ports to hardware buttons depends on the code in the driver station.
 */
class Joystick : public GenericHID, public ErrorBase
{
public:
	static const uint32_t kDefaultXAxis = 0;
	static const uint32_t kDefaultYAxis = 1;
	static const uint32_t kDefaultZAxis = 2;
	static const uint32_t kDefaultTwistAxis = 2;
	static const uint32_t kDefaultThrottleAxis = 3;
	typedef enum
	{
		kXAxis, kYAxis, kZAxis, kTwistAxis, kThrottleAxis, kNumAxisTypes
	} AxisType;
	static const uint32_t kDefaultTriggerButton = 1;
	static const uint32_t kDefaultTopButton = 2;
	typedef enum
	{
		kTriggerButton, kTopButton, kNumButtonTypes
	} ButtonType;
	typedef enum
	{
		kLeftRumble, kRightRumble
	} RumbleType;

	explicit Joystick(uint32_t port);
	Joystick(uint32_t port, uint32_t numAxisTypes, uint32_t numButtonTypes);
	virtual ~Joystick();

	uint32_t GetAxisChannel(AxisType axis);
	void SetAxisChannel(AxisType axis, uint32_t channel);

	virtual float GetX(JoystickHand hand = kRightHand);
	virtual float GetY(JoystickHand hand = kRightHand);
	virtual float GetZ();
	virtual float GetTwist();
	virtual float GetThrottle();
	virtual float GetAxis(AxisType axis);
	float GetRawAxis(uint32_t axis);

	virtual bool GetTrigger(JoystickHand hand = kRightHand);
	virtual bool GetTop(JoystickHand hand = kRightHand);
	virtual bool GetBumper(JoystickHand hand = kRightHand);
	virtual bool GetRawButton(uint32_t button);
	virtual int GetPOV(uint32_t pov = 0);
	bool GetButton(ButtonType button);
	static Joystick* GetStickForPort(uint32_t port);

	virtual float GetMagnitude();
	virtual float GetDirectionRadians();
	virtual float GetDirectionDegrees();

	int GetAxisCount();
	int GetButtonCount();
	int GetPOVCount();
	
	void SetRumble(RumbleType type, float value);
	void SetOutput(uint8_t outputNumber, bool value);
	void SetOutputs(uint32_t value);

private:
	DISALLOW_COPY_AND_ASSIGN(Joystick);
	void InitJoystick(uint32_t numAxisTypes, uint32_t numButtonTypes);

	DriverStation *m_ds;
	uint32_t m_port;
	uint32_t *m_axes;
	uint32_t *m_buttons;
	uint32_t m_outputs;
	uint16_t m_leftRumble;
	uint16_t m_rightRumble;
};

#endif
