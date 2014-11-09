/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CANTalon.h"
#include "WPIErrors.h"
#include "ctre/CanTalonSRX.h"

/**
 * Constructor for the CANTalon device.
 * @param deviceNumber The CAN ID of the Talon SRX
 */
CANTalon::CANTalon(uint8_t deviceNumber)
	: m_deviceNumber(deviceNumber)
	, m_impl(new CanTalonSRX(deviceNumber))
	, m_safetyHelper(new MotorSafetyHelper(this))
{
}

CANTalon::~CANTalon() {
	delete m_impl;
	delete m_safetyHelper;
}

/**
* Write out the PID value as seen in the PIDOutput base object.
*
* @deprecated Call Set instead.
*
* @param output Write out the PercentVbus value as was computed by the PIDController
*/
void CANTalon::PIDWrite(float output)
{
	if (GetControlMode() == kPercentVbus)
	{
		Set(output);
	}
	else
	{
		wpi_setWPIErrorWithContext(IncompatibleMode, "PID only supported in PercentVbus mode");
	}
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::Get()
{
	// TODO
	return 0.0f;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::Set(float value, uint8_t syncGroup)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::Disable()
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetP(double p)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetI(double i)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetD(double d)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetPID(double p, double i, double d)
{
	SetP(p);
	SetI(i);
	SetD(d);
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
double CANTalon::GetP()
{
	// TODO
	return 0.0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
double CANTalon::GetI()
{
	// TODO
	return 0.0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
double CANTalon::GetD()
{
	// TODO
	return 0.0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetBusVoltage()
{
	// TODO
	return 0.0f;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetOutputVoltage()
{
	// TODO
	return 0.0f;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetOutputCurrent()
{
	// TODO
	return 0.0f;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetTemperature()
{
	// TODO
	return 0.0f;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
double CANTalon::GetPosition()
{
	// TODO
	return 0.0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
double CANTalon::GetSpeed()
{
	// TODO
	return 0.0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
bool CANTalon::GetForwardLimitOK()
{
	// TODO
	return false;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
bool CANTalon::GetReverseLimitOK()
{
	// TODO
	return false;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
uint16_t CANTalon::GetFaults()
{
	// TODO
	return 0;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetVoltageRampRate(double rampRate)
{
	// TODO
}

/**
 * @return The version of the firmware running on the Talon
 */
uint32_t CANTalon::GetFirmwareVersion()
{
	int firmwareVersion;

	CTR_Code status = m_impl->GetFirmVers(firmwareVersion);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}

	return firmwareVersion;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigNeutralMode(NeutralMode mode)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigEncoderCodesPerRev(uint16_t codesPerRev)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigPotentiometerTurns(uint16_t turns)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::DisableSoftPositionLimits()
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigLimitMode(LimitMode mode)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigForwardLimit(double forwardLimitPosition)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigReverseLimit(double reverseLimitPosition)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigMaxOutputVoltage(double voltage)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigFaultTime(float faultTime)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetControlMode(CANSpeedController::ControlMode mode)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
CANSpeedController::ControlMode CANTalon::GetControlMode()
{
	// TODO
	return CANSpeedController::ControlMode::kPercentVbus;
}

void CANTalon::SetExpiration(float timeout)
{
	m_safetyHelper->SetExpiration(timeout);
}

float CANTalon::GetExpiration()
{
	return m_safetyHelper->GetExpiration();
}

bool CANTalon::IsAlive()
{
	return m_safetyHelper->IsAlive();
}

bool CANTalon::IsSafetyEnabled()
{
	return m_safetyHelper->IsSafetyEnabled();
}

void CANTalon::SetSafetyEnabled(bool enabled)
{
	m_safetyHelper->SetSafetyEnabled(enabled);
}

void CANTalon::GetDescription(char *desc)
{
	sprintf(desc, "CANTalon ID %d", m_deviceNumber);
}

/**
* Common interface for stopping the motor
* Part of the MotorSafety interface
*/
void CANTalon::StopMotor()
{
	// TODO
}
