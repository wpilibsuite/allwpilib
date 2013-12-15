/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Victor.h"

#include "DigitalModule.h"
#include "NetworkCommunication/UsageReporting.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common initialization code called by all constructors.
 *
 * Note that the Victor uses the following bounds for PWM values.  These values were determined
 * empirically and optimized for the Victor 888. These values should work reasonably well for
 * Victor 884 controllers as well but if users experience issues such as asymmetric behavior around
 * the deadband or inability to saturate the controller in either direction, calibration is recommended.
 * The calibration procedure can be found in the Victor 884 User Manual available from IFI.
 * 
 *   - 206 = full "forward"
 *   - 131 = the "high end" of the deadband range
 *   - 128 = center of the deadband range (off)
 *   - 125 = the "low end" of the deadband range
 *   - 56 = full "reverse"
 */
void Victor::InitVictor() {
	SetBounds(2.027, 1.525, 1.507, 1.49, 1.026);
	
	SetPeriodMultiplier(kPeriodMultiplier_2X);
	SetRaw(m_centerPwm);

	LiveWindow::GetInstance()->AddActuator("Victor", GetModuleNumber(), GetChannel(), this);
	nUsageReporting::report(nUsageReporting::kResourceType_Victor, GetChannel(), GetModuleNumber() - 1);
}

/**
 * Constructor that assumes the default digital module.
 * 
 * @param channel The PWM channel on the digital module that the Victor is attached to.
 */
Victor::Victor(uint32_t channel) : SafePWM(channel)
{
	InitVictor();
}

/**
 * Constructor that specifies the digital module.
 * 
 * @param moduleNumber The digital module (1 or 2).
 * @param channel The PWM channel on the digital module that the Victor is attached to (1..10).
 */
Victor::Victor(uint8_t moduleNumber, uint32_t channel) : SafePWM(moduleNumber, channel)
{
	InitVictor();
}

Victor::~Victor()
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
void Victor::Set(float speed, uint8_t syncGroup)
{
	SetSpeed(speed);
}

/**
 * Get the recently set value of the PWM.
 * 
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Victor::Get()
{
	return GetSpeed();
}

/**
 * Common interface for disabling a motor.
 */
void Victor::Disable()
{
	SetRaw(kPwmDisabled);
}

/**
 * Write out the PID value as seen in the PIDOutput base object.
 * 
 * @param output Write out the PWM value as was found in the PIDController
 */
void Victor::PIDWrite(float output) 
{
	Set(output);
}

