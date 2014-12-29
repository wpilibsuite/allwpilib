/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Talon.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common initialization code called by all constructors.
 *
 * Note that the Talon uses the following bounds for PWM values. These values should work reasonably well for
 * most controllers, but if users experience issues such as asymmetric behavior around
 * the deadband or inability to saturate the controller in either direction, calibration is recommended.
 * The calibration procedure can be found in the Talon User Manual available from CTRE.
 *
 *   2.037ms = full "forward"
 *   1.539ms = the "high end" of the deadband range
 *   1.513ms = center of the deadband range (off)
 *   1.487ms = the "low end" of the deadband range
 *   0.989ms = full "reverse"
 */
void Talon::InitTalon() {
	SetBounds(2.037, 1.539, 1.513, 1.487, .989);
	SetPeriodMultiplier(kPeriodMultiplier_1X);
	SetRaw(m_centerPwm);
	SetZeroLatch();

	HALReport(HALUsageReporting::kResourceType_Talon, GetChannel());
	LiveWindow::GetInstance()->AddActuator("Talon", GetChannel(), this);
}

/**
 * Constructor for a Talon (original or Talon SR)
 * @param channel The PWM channel number that the Talon is attached to. 0-9 are on-board, 10-19 are on the MXP port
 */
Talon::Talon(uint32_t channel) : SafePWM(channel)
{
	InitTalon();
}

Talon::~Talon()
{
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 * @param syncGroup Unused interface.
 */
void Talon::Set(float speed, uint8_t syncGroup)
{
	SetSpeed(speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Talon::Get()
{
	return GetSpeed();
}

/**
 * Common interface for disabling a motor.
 */
void Talon::Disable()
{
	SetRaw(kPwmDisabled);
}

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Talon::PIDWrite(float output)
{
	Set(output);
}
