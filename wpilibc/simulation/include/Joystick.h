/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <memory>
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
	static const uint32_t kDefaultXAxis = 1;
	static const uint32_t kDefaultYAxis = 2;
	static const uint32_t kDefaultZAxis = 3;
	static const uint32_t kDefaultTwistAxis = 4;
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

	explicit Joystick(uint32_t port);
	Joystick(uint32_t port, uint32_t numAxisTypes, uint32_t numButtonTypes);
	virtual ~Joystick() = default;

    Joystick(const Joystick&) = delete;
    Joystick& operator=(const Joystick&) = delete;

	uint32_t GetAxisChannel(AxisType axis);
	void SetAxisChannel(AxisType axis, uint32_t channel);

	virtual float GetX(JoystickHand hand = kRightHand) const override;
	virtual float GetY(JoystickHand hand = kRightHand) const override;
	virtual float GetZ() const override;
	virtual float GetTwist() const override;
	virtual float GetThrottle() const override;
	virtual float GetAxis(AxisType axis) const;
	float GetRawAxis(uint32_t axis) const override;

	virtual bool GetTrigger(JoystickHand hand = kRightHand) const override;
	virtual bool GetTop(JoystickHand hand = kRightHand) const override;
	virtual bool GetBumper(JoystickHand hand = kRightHand) const override;
	virtual bool GetRawButton(uint32_t button) const override;
	virtual int GetPOV(uint32_t pov = 1) const override;
	bool GetButton(ButtonType button) const;
	static Joystick* GetStickForPort(uint32_t port);

	virtual float GetMagnitude() const;
	virtual float GetDirectionRadians() const;
	virtual float GetDirectionDegrees() const;

private:
	DriverStation &m_ds;
	uint32_t m_port;
	std::unique_ptr<uint32_t[]> m_axes;
	std::unique_ptr<uint32_t[]> m_buttons;
};

#endif
