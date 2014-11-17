/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "CANTalon.h"
#include "WPIErrors.h"
#include "ctre/CanTalonSRX.h"

/**
 * The CANTalon object is currently incomplete. As of Nov 14 2014, we only know
 * for sure that sending a throttle and checking basic values (eg current,
 * temperature) work.
 */

/**
 * Constructor for the CANTalon device.
 * @param deviceNumber The CAN ID of the Talon SRX
 */
CANTalon::CANTalon(int deviceNumber)
	: m_deviceNumber(deviceNumber)
	, m_impl(new CanTalonSRX(deviceNumber))
	, m_safetyHelper(new MotorSafetyHelper(this))
  , m_controlEnabled(false)
{
  // The control mode may already have been set.
  CTR_Code status = m_impl->SetModeSelect((int)m_controlMode);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
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
  return 0.0f;
}

/**
 * Sets the output set-point value.
 */
void CANTalon::Set(float value, uint8_t syncGroup)
{
  if(m_controlEnabled) {
    CTR_Code status;
    switch(GetControlMode()) {
      case kThrottle:
        {
          m_impl->Set(value);
          status = CTR_OKAY;
        }
        break;
      case kFollowerMode:
        {
          status = m_impl->SetDemand24((int)value);
        }
        break;
      case kVoltageMode:
        {
          // Voltage is an 8.8 fixed point number.
          int volts = int(value * 256);
          status = m_impl->SetDemand24(volts);
        }
      default:
        // TODO: Add support for other modes. Need to figure out what format
        // SetDemand24 needs.
        break;
    }
    if (status != CTR_OKAY) {
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
    }
  }
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::Disable()
{
  // Until Modes other than throttle work, just disable by setting throttle to 0.0.
  m_impl->Set(0.0); // TODO when firmware is updated, remove this.
  //m_impl->SetModeSelect(kDisabled); // TODO when firmware is updated, uncomment this.
  m_controlEnabled = false;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::EnableControl() {
  SetControlMode(m_controlMode);
  m_controlEnabled = true;
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
 * Returns the voltage coming in from the battery.
 *
 * @return The input voltage in vols.
 */
float CANTalon::GetBusVoltage()
{
  double voltage;
  CTR_Code status = m_impl->GetBatteryV(voltage);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
  return voltage;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetOutputVoltage()
{
  int throttle11;
  CTR_Code status = m_impl->GetAppliedThrottle11(throttle11);
  float voltage = GetBusVoltage() * float(throttle11) / 1023.0;
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
	return voltage;
}


/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetOutputCurrent()
{
  double current;

  CTR_Code status = m_impl->GetCurrent(current);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}

	return current;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
float CANTalon::GetTemperature()
{
  double temp;

  CTR_Code status = m_impl->GetTemp(temp);
	if(temp != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
	return temp;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 *
 * @return The position of the sensor currently providing feedback.
 */
double CANTalon::GetPosition()
{
  int postition;
  // TODO convert from int to appropriate units (or at least document it).

  CTR_Code status = m_impl->GetSensorPosition(postition);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
	return (double)postition;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 *
 * @returns The speed of the sensor currently providing feedback.
 */
double CANTalon::GetSpeed()
{
  int speed;
  // TODO convert from int to appropriate units (or at least document it).

  CTR_Code status = m_impl->GetSensorVelocity(speed);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
	return (double)speed;
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
 * Does this exist on the Talon?
 */
void CANTalon::ConfigMaxOutputVoltage(double voltage)
{
	// TODO
}

/**
 * TODO documentation (see CANJaguar.cpp)
 * Does this exist on the Talon?
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
  m_controlMode = mode;
  TalonControlMode sendMode;
  switch (mode) {
    case kPercentVbus:
      sendMode = kThrottle;
      break;
    case kCurrent:
      sendMode = kCurrentMode;
      break;
    case kSpeed:
      sendMode = kSpeedMode;
      break;
    case kPosition:
      sendMode = kPositionMode;
      break;
    case kVoltage:
      sendMode = kVoltageMode;
      break;
    case kFollower:
      sendMode = kFollowerMode;
      break;
  }
  CTR_Code status = m_impl->SetModeSelect((int)sendMode);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
CANSpeedController::ControlMode CANTalon::GetControlMode()
{
  // Take the opportunity to check that the control mode is what we think it is.
  int temp;
  CTR_Code status = m_impl->GetModeSelect(temp);
	if(status != CTR_OKAY) {
		wpi_setErrorWithContext(status, getHALErrorMessage(status));
	}
  switch ((TalonControlMode)temp) {
    case kThrottle:
      m_controlMode = kPercentVbus;
      break;
    case kCurrentMode:
      m_controlMode = kCurrent;
      break;
    case kSpeedMode:
      m_controlMode = kSpeed;
      break;
    case kPositionMode:
      m_controlMode = kPosition;
      break;
    case kVoltageMode:
      m_controlMode = kVoltage;
      break;
    case kFollowerMode:
      m_controlMode = kFollower;
      break;
    case kDisabled:
      m_controlEnabled = false;
      break;
  }
  return m_controlMode;
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
 *
 * @deprecated Call Disable instead.
*/
void CANTalon::StopMotor()
{
  Disable();
}
