/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CANTalon.h"
#include "WPIErrors.h"
#include <unistd.h>  // usleep
#include <sstream>
#include "LiveWindow/LiveWindow.h"

/**
 * Number of adc engineering units per 0 to 3.3V sweep.
 * This is necessary for scaling Analog Position in rotations/RPM.
 */
const double kNativeAdcUnitsPerRotation = 1024.0;
/**
 * Number of pulse width engineering units per full rotation.
 * This is necessary for scaling Pulse Width Decoded Position in rotations/RPM.
 */
const double kNativePwdUnitsPerRotation = 4096.0;
/**
 * Number of minutes per 100ms unit.  Useful for scaling velocities
 * measured by Talon's 100ms timebase to rotations per minute.
 */
const double kMinutesPer100msUnit = 1.0/600.0;

/**
 * Constructor for the CANTalon device.
 * @param deviceNumber The CAN ID of the Talon SRX
 */
CANTalon::CANTalon(int deviceNumber)
    : m_deviceNumber(deviceNumber),
      m_impl(new CanTalonSRX(deviceNumber)),
      m_safetyHelper(new MotorSafetyHelper(this)) {
  ApplyControlMode(m_controlMode);
  m_impl->SetProfileSlotSelect(m_profile);
  m_isInverted = false;
  LiveWindow::GetInstance()->AddActuator("CANTalon", m_deviceNumber, this);
}
/**
 * Constructor for the CANTalon device.
 * @param deviceNumber The CAN ID of the Talon SRX
 * @param controlPeriodMs The period in ms to send the CAN control frame.
 *                        Period is bounded to [1ms,95ms].
 */
CANTalon::CANTalon(int deviceNumber, int controlPeriodMs)
    : m_deviceNumber(deviceNumber),
      m_impl(new CanTalonSRX(deviceNumber,controlPeriodMs)),
      m_safetyHelper(new MotorSafetyHelper(this)) {
  ApplyControlMode(m_controlMode);
  m_impl->SetProfileSlotSelect(m_profile);
  LiveWindow::GetInstance()->AddActuator("CANTalon", m_deviceNumber, this);
}

CANTalon::~CANTalon() {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
  if (m_hasBeenMoved) return;
  Disable();
}

/**
* Write out the PID value as seen in the PIDOutput base object.
*
* @deprecated Call Set instead.
*
* @param output Write out the PercentVbus value as was computed by the
* PIDController
*/
void CANTalon::PIDWrite(float output) {
  if (GetControlMode() == kPercentVbus) {
    Set(output);
  } else {
    wpi_setWPIErrorWithContext(IncompatibleMode,
                               "PID only supported in PercentVbus mode");
  }
}

/**
 * Retrieve the current sensor value. Equivalent to Get().
 *
 * @return The current sensor value of the Talon.
 */
double CANTalon::PIDGet() { return Get(); }

/**
 * Gets the current status of the Talon (usually a sensor value).
 *
 * In Current mode: returns output current.
 * In Speed mode: returns current speed.
 * In Position mode: returns current sensor position.
 * In PercentVbus and Follower modes: returns current applied throttle.
 *
 * @return The current sensor value of the Talon.
 */
float CANTalon::Get() const {
  int value;
  switch (m_controlMode) {
    case kVoltage:
      return GetOutputVoltage();
    case kCurrent:
      return GetOutputCurrent();
    case kSpeed:
      m_impl->GetSensorVelocity(value);
      return ScaleNativeUnitsToRpm(m_feedbackDevice, value);
    case kPosition:
      m_impl->GetSensorPosition(value);
      return ScaleNativeUnitsToRotations(m_feedbackDevice, value);
    case kPercentVbus:
    case kFollower:
    default:
      m_impl->GetAppliedThrottle(value);
      return (float)value / 1023.0;
  }
}

/**
 * Sets the appropriate output on the talon, depending on the mode.
 *
 * In PercentVbus, the output is between -1.0 and 1.0, with 0.0 as stopped.
 * In Voltage mode, output value is in volts.
 * In Current mode, output value is in amperes.
 * In Speed mode, output value is in position change / 10ms.
 * In Position mode, output value is in encoder ticks or an analog value,
 *   depending on the sensor.
 * In Follower mode, the output value is the integer device ID of the talon to
 * duplicate.
 *
 * @param outputValue The setpoint value, as described above.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::Set(float value, uint8_t syncGroup) {
  /* feed safety helper since caller just updated our output */
  m_safetyHelper->Feed();

  if (m_stopped) {
    EnableControl();
    m_stopped = false;
  }

  if (m_controlEnabled) {
    m_setPoint = value;  /* cache set point for GetSetpoint() */
    CTR_Code status = CTR_OKAY;
    switch (m_controlMode) {
      case CANSpeedController::kPercentVbus: {
        m_impl->Set(m_isInverted ? -value : value);
        status = CTR_OKAY;
      } break;
      case CANSpeedController::kFollower: {
        status = m_impl->SetDemand((int)value);
      } break;
      case CANSpeedController::kVoltage: {
        // Voltage is an 8.8 fixed point number.
        int volts = int((m_isInverted ? -value : value) * 256);
        status = m_impl->SetDemand(volts);
      } break;
      case CANSpeedController::kSpeed:
        /* if the caller has provided scaling info, apply it */
        status = m_impl->SetDemand(ScaleVelocityToNativeUnits(m_feedbackDevice, m_isInverted ? -value : value));
        break;
      case CANSpeedController::kPosition:
        status = m_impl->SetDemand(ScaleRotationsToNativeUnits(m_feedbackDevice, value));
        break;
      case CANSpeedController::kCurrent: {
        double milliamperes = (m_isInverted ? -value : value) * 1000.0; /* mA*/
        status = m_impl->SetDemand(milliamperes);
      } break;
      case CANSpeedController::kMotionProfile: {
        status = m_impl->SetDemand((int)value);
      } break;
      default:
        wpi_setWPIErrorWithContext(
            IncompatibleMode,
            "The CAN Talon does not support this control mode.");
        break;
    }
    if (status != CTR_OKAY) {
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
    }

    status = m_impl->SetModeSelect(m_sendMode);

    if (status != CTR_OKAY) {
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
    }
  }
}

/**
 * Sets the setpoint to value. Equivalent to Set().
 */
void CANTalon::SetSetpoint(float value) { Set(value); }

/**
 * Resets the integral term and disables the controller.
 */
void CANTalon::Reset() {
  ClearIaccum();
  Disable();
}

/**
 * Disables control of the talon, causing the motor to brake or coast
 * depending on its mode (see the Talon SRX Software Reference manual
 * for more information).
 */
void CANTalon::Disable() {
  m_impl->SetModeSelect((int)CANTalon::kDisabled);
  m_controlEnabled = false;
}

/**
 * Enables control of the Talon, allowing the motor to move.
 */
void CANTalon::EnableControl() {
  SetControlMode(m_controlMode);
  m_controlEnabled = true;
}

/**
 * Enables control of the Talon, allowing the motor to move.
 */
void CANTalon::Enable() { EnableControl(); }

/**
 * @return Whether the Talon is currently enabled.
 */
bool CANTalon::IsControlEnabled() const { return m_controlEnabled; }

/**
 * @return Whether the Talon is currently enabled.
 */
bool CANTalon::IsEnabled() const { return IsControlEnabled(); }

/**
 * @param p Proportional constant to use in PID loop.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::SetP(double p) {
  CTR_Code status = m_impl->SetPgain(m_profile, p);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Set the integration constant of the currently selected profile.
 *
 * @param i Integration constant for the currently selected PID profile.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::SetI(double i) {
  CTR_Code status = m_impl->SetIgain(m_profile, i);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Set the derivative constant of the currently selected profile.
 *
 * @param d Derivative constant for the currently selected PID profile.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::SetD(double d) {
  CTR_Code status = m_impl->SetDgain(m_profile, d);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Set the feedforward value of the currently selected profile.
 *
 * @param f Feedforward constant for the currently selected PID profile.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::SetF(double f) {
  CTR_Code status = m_impl->SetFgain(m_profile, f);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Set the Izone to a nonzero value to auto clear the integral accumulator
 *     when the absolute value of CloseLoopError exceeds Izone.
 *
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
void CANTalon::SetIzone(unsigned iz) {
  CTR_Code status = m_impl->SetIzone(m_profile, iz);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * SRX has two available slots for PID.
 * @param slotIdx one or zero depending on which slot caller wants.
 */
void CANTalon::SelectProfileSlot(int slotIdx) {
  m_profile = (slotIdx == 0) ? 0 : 1; /* only get two slots for now */
  CTR_Code status = m_impl->SetProfileSlotSelect(m_profile);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Sets control values for closed loop control.
 *
 * @param p Proportional constant.
 * @param i Integration constant.
 * @param d Differential constant.
 * This function does not modify F-gain.  Considerable passing a zero for f
 * using
 * the four-parameter function.
 */
void CANTalon::SetPID(double p, double i, double d) {
  SetP(p);
  SetI(i);
  SetD(d);
}
/**
 * Sets control values for closed loop control.
 *
 * @param p Proportional constant.
 * @param i Integration constant.
 * @param d Differential constant.
 * @param f Feedforward constant.
 */
void CANTalon::SetPID(double p, double i, double d, double f) {
  SetP(p);
  SetI(i);
  SetD(d);
  SetF(f);
}
/**
 * Select the feedback device to use in closed-loop
 */
void CANTalon::SetFeedbackDevice(FeedbackDevice feedbackDevice) {
  /* save the selection so that future setters/getters know which scalars to apply */
  m_feedbackDevice = feedbackDevice;
  /* pass feedback to actual CAN frame */
  CTR_Code status = m_impl->SetFeedbackDeviceSelect((int)feedbackDevice);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Select the feedback device to use in closed-loop
 */
void CANTalon::SetStatusFrameRateMs(StatusFrameRate stateFrame, int periodMs) {
  CTR_Code status = m_impl->SetStatusFrameRate((int)stateFrame, periodMs);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Get the current proportional constant.
 *
 * @return double proportional constant for current profile.
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
double CANTalon::GetP() const {
  CanTalonSRX::param_t param = m_profile ? CanTalonSRX::eProfileParamSlot1_P
                                         : CanTalonSRX::eProfileParamSlot0_P;
  // Update the info in m_impl.
  CTR_Code status = m_impl->RequestParam(param);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs); /* small yield for getting response */
  double p;
  status = m_impl->GetPgain(m_profile, p);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return p;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
double CANTalon::GetI() const {
  CanTalonSRX::param_t param = m_profile ? CanTalonSRX::eProfileParamSlot1_I
                                         : CanTalonSRX::eProfileParamSlot0_I;
  // Update the info in m_impl.
  CTR_Code status = m_impl->RequestParam(param);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs); /* small yield for getting response */

  double i;
  status = m_impl->GetIgain(m_profile, i);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return i;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
double CANTalon::GetD() const {
  CanTalonSRX::param_t param = m_profile ? CanTalonSRX::eProfileParamSlot1_D
                                         : CanTalonSRX::eProfileParamSlot0_D;
  // Update the info in m_impl.
  CTR_Code status = m_impl->RequestParam(param);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs); /* small yield for getting response */

  double d;
  status = m_impl->GetDgain(m_profile, d);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return d;
}
/**
 *
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
double CANTalon::GetF() const {
  CanTalonSRX::param_t param = m_profile ? CanTalonSRX::eProfileParamSlot1_F
                                         : CanTalonSRX::eProfileParamSlot0_F;
  // Update the info in m_impl.
  CTR_Code status = m_impl->RequestParam(param);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }

  usleep(kDelayForSolicitedSignalsUs); /* small yield for getting response */
  double f;
  status = m_impl->GetFgain(m_profile, f);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return f;
}
/**
 * @see SelectProfileSlot to choose between the two sets of gains.
 */
int CANTalon::GetIzone() const {
  CanTalonSRX::param_t param = m_profile
                                   ? CanTalonSRX::eProfileParamSlot1_IZone
                                   : CanTalonSRX::eProfileParamSlot0_IZone;
  // Update the info in m_impl.
  CTR_Code status = m_impl->RequestParam(param);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs);

  int iz;
  status = m_impl->GetIzone(m_profile, iz);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return iz;
}

/**
 * @return the current setpoint; ie, whatever was last passed to Set().
 */
double CANTalon::GetSetpoint() const { return m_setPoint; }

/**
 * Returns the voltage coming in from the battery.
 *
 * @return The input voltage in volts.
 */
float CANTalon::GetBusVoltage() const {
  double voltage;
  CTR_Code status = m_impl->GetBatteryV(voltage);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return voltage;
}

/**
 * @return The voltage being output by the Talon, in Volts.
 */
float CANTalon::GetOutputVoltage() const {
  int throttle11;
  CTR_Code status = m_impl->GetAppliedThrottle(throttle11);
  float voltage = GetBusVoltage() * (float(throttle11) / 1023.0);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return voltage;
}

/**
 *  Returns the current going through the Talon, in Amperes.
 */
float CANTalon::GetOutputCurrent() const {
  double current;

  CTR_Code status = m_impl->GetCurrent(current);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }

  return current;
}

/**
 *  Returns temperature of Talon, in degrees Celsius.
 */
float CANTalon::GetTemperature() const {
  double temp;

  CTR_Code status = m_impl->GetTemp(temp);
  if (temp != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return temp;
}
/**
 * Set the position value of the selected sensor.  This is useful for zero-ing
 * quadrature encoders.
 * Continuous sensors (like analog encoderes) can also partially be set (the
 * portion of the postion based on overflows).
 */
void CANTalon::SetPosition(double pos) {
  int32_t nativePos = ScaleRotationsToNativeUnits(m_feedbackDevice, pos);
  CTR_Code status = m_impl->SetSensorPosition(nativePos);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * TODO documentation (see CANJaguar.cpp)
 *
 * @return The position of the sensor currently providing feedback.
 *       When using analog sensors, 0 units corresponds to 0V, 1023
 * units corresponds to 3.3V
 *       When using an analog encoder (wrapping around 1023 => 0 is
 * possible) the units are still 3.3V per 1023 units.
 *       When using quadrature, each unit is a quadrature edge (4X)
 * mode.
 */
double CANTalon::GetPosition() const {
  int32_t position;
  CTR_Code status = m_impl->GetSensorPosition(position);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return ScaleNativeUnitsToRotations(m_feedbackDevice, position);
}
/**
 * If sensor and motor are out of phase, sensor can be inverted
 * (position and velocity multiplied by -1).
 * @see GetPosition and @see GetSpeed.
 */
void CANTalon::SetSensorDirection(bool reverseSensor) {
  CTR_Code status = m_impl->SetRevFeedbackSensor(reverseSensor ? 1 : 0);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Flips the sign (multiplies by negative one) the throttle values going into
 * the motor on the talon in closed loop modes.  Typically the application
 * should use SetSensorDirection to keep sensor and motor in phase.
 * @see SetSensorDirection
 * However this routine is helpful for reversing the motor direction
 * when Talon is in slave mode, or when using a single-direction position
 * sensor in a closed-loop mode.
 *
 * @param reverseOutput True if motor output should be flipped; False if not.
 */
void CANTalon::SetClosedLoopOutputDirection(bool reverseOutput) {
  CTR_Code status = m_impl->SetRevMotDuringCloseLoopEn(reverseOutput ? 1 : 0);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Returns the current error in the controller.
 *
 * @return the difference between the setpoint and the sensor value.
 */
int CANTalon::GetClosedLoopError() const {
  int error;
  /* retrieve the closed loop error in native units */
  CTR_Code status = m_impl->GetCloseLoopErr(error);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return error;
}
/**
 * Set the allowable closed loop error.
 * @param allowableCloseLoopError allowable closed loop error for selected profile.
 *       mA for Curent closed loop.
 *       Talon Native Units for position and velocity.
 */
void CANTalon::SetAllowableClosedLoopErr(uint32_t allowableCloseLoopError)
{
  /* grab param enum */
  CanTalonSRX::param_t param;
  if (m_profile == 1) {
    param = CanTalonSRX::eProfileParamSlot1_AllowableClosedLoopErr;
  } else {
    param = CanTalonSRX::eProfileParamSlot0_AllowableClosedLoopErr;
  }
  /* send allowable close loop er in native units */
  ConfigSetParameter(param, allowableCloseLoopError);
}

/**
 * TODO documentation (see CANJaguar.cpp)
 *
 * @returns The speed of the sensor currently providing feedback.
 *
 * The speed units will be in the sensor's native ticks per 100ms.
 *
 * For analog sensors, 3.3V corresponds to 1023 units.
 *     So a speed of 200 equates to ~0.645 dV per 100ms or 6.451 dV per
 * second.
 *     If this is an analog encoder, that likely means 1.9548 rotations
 * per sec.
 * For quadrature encoders, each unit corresponds a quadrature edge (4X).
 *     So a 250 count encoder will produce 1000 edge events per
 * rotation.
 *     An example speed of 200 would then equate to 20% of a rotation
 * per 100ms,
 *     or 10 rotations per second.
 */
double CANTalon::GetSpeed() const {
  int32_t speed;
  CTR_Code status = m_impl->GetSensorVelocity(speed);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return ScaleNativeUnitsToRpm(m_feedbackDevice, speed);
}

/**
 * Get the position of whatever is in the analog pin of the Talon, regardless of
 * whether it is actually being used for feedback.
 *
 * @returns The 24bit analog value.  The bottom ten bits is the ADC (0 - 1023)
 *          on the analog pin of the Talon.
 *          The upper 14 bits tracks the overflows and
 * underflows (continuous sensor).
 */
int CANTalon::GetAnalogIn() const {
  int position;
  CTR_Code status = m_impl->GetAnalogInWithOv(position);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return position;
}

void CANTalon::SetAnalogPosition(int newPosition) {
  CTR_Code status = m_impl->SetParam(CanTalonSRX::eAinPosition, newPosition);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Get the position of whatever is in the analog pin of the Talon, regardless of
 * whether it is actually being used for feedback.
 *
 * @returns The ADC (0 - 1023) on analog pin of the Talon.
 */
int CANTalon::GetAnalogInRaw() const { return GetAnalogIn() & 0x3FF; }
/**
 * Get the position of whatever is in the analog pin of the Talon, regardless of
 * whether it is actually being used for feedback.
 *
 * @returns The value (0 - 1023) on the analog pin of the Talon.
 */
int CANTalon::GetAnalogInVel() const {
  int vel;
  CTR_Code status = m_impl->GetAnalogInVel(vel);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return vel;
}

/**
 * Get the position of whatever is in the analog pin of the Talon, regardless of
 * whether it is actually being used for feedback.
 *
 * @returns The value (0 - 1023) on the analog pin of the Talon.
 */
int CANTalon::GetEncPosition() const {
  int position;
  CTR_Code status = m_impl->GetEncPosition(position);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return position;
}
void CANTalon::SetEncPosition(int newPosition) {
  CTR_Code status = m_impl->SetParam(CanTalonSRX::eEncPosition, newPosition);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Get the position of whatever is in the analog pin of the Talon, regardless of
 * whether it is actually being used for feedback.
 *
 * @returns The value (0 - 1023) on the analog pin of the Talon.
 */
int CANTalon::GetEncVel() const {
  int vel;
  CTR_Code status = m_impl->GetEncVel(vel);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return vel;
}
int CANTalon::GetPulseWidthPosition() const {
  int param;
  CTR_Code status = m_impl->GetPulseWidthPosition(param);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return param;
}
void CANTalon::SetPulseWidthPosition(int newPosition)
{
  CTR_Code status = m_impl->SetParam(CanTalonSRX::ePwdPosition, newPosition);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
int CANTalon::GetPulseWidthVelocity()const
{
  int param;
  CTR_Code status = m_impl->GetPulseWidthVelocity(param);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return param;
}
int CANTalon::GetPulseWidthRiseToFallUs()const
{
  int param;
  CTR_Code status = m_impl->GetPulseWidthRiseToFallUs(param);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return param;
}
int CANTalon::GetPulseWidthRiseToRiseUs()const
{
  int param;
  CTR_Code status = m_impl->GetPulseWidthRiseToRiseUs(param);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return param;
}
/**
 * @param which feedback sensor to check it if is connected.
 * @return status of caller's specified sensor type.
 */
CANTalon::FeedbackDeviceStatus CANTalon::IsSensorPresent(FeedbackDevice feedbackDevice)const
{
  FeedbackDeviceStatus retval = FeedbackStatusUnknown;
  int param;
  /* detecting sensor health depends on which sensor caller cares about */
  switch (feedbackDevice) {
    case QuadEncoder:
    case AnalogPot:
    case AnalogEncoder:
    case EncRising:
    case EncFalling:
      /* no real good way to tell if these sensor
        are actually present so return status unknown. */
      break;
    case PulseWidth:
    case CtreMagEncoder_Relative:
    case CtreMagEncoder_Absolute:
      /* all of these require pulse width signal to be present. */
      CTR_Code status = m_impl->IsPulseWidthSensorPresent(param);
      if (status != CTR_OKAY) {
        /* we're not getting status info, signal unknown status */
      } else {
        /* param is updated */
        if (param) {
          /* pulse signal is present, sensor must be working since it always
            generates a pulse waveform.*/
          retval = FeedbackStatusPresent;
        } else {
          /* no pulse present, sensor disconnected */
          retval = FeedbackStatusNotPresent;
        }
      }
      break;
  }
  return retval;
}
/**
 * @return IO level of QUADA pin.
 */
int CANTalon::GetPinStateQuadA() const {
  int retval;
  CTR_Code status = m_impl->GetQuadApin(retval);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return retval;
}
/**
 * @return IO level of QUADB pin.
 */
int CANTalon::GetPinStateQuadB() const {
  int retval;
  CTR_Code status = m_impl->GetQuadBpin(retval);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return retval;
}
/**
 * @return IO level of QUAD Index pin.
 */
int CANTalon::GetPinStateQuadIdx() const {
  int retval;
  CTR_Code status = m_impl->GetQuadIdxpin(retval);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return retval;
}
/**
 * @return '1' iff forward limit switch is closed, 0 iff switch is open.
 * This function works regardless if limit switch feature is enabled.
 */
int CANTalon::IsFwdLimitSwitchClosed() const {
  int retval;
  CTR_Code status = m_impl->GetLimitSwitchClosedFor(
      retval); /* rename this func, '1' => open, '0' => closed */
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return retval ? 0 : 1;
}
/**
 * @return '1' iff reverse limit switch is closed, 0 iff switch is open.
 * This function works regardless if limit switch feature is enabled.
 */
int CANTalon::IsRevLimitSwitchClosed() const {
  int retval;
  CTR_Code status = m_impl->GetLimitSwitchClosedRev(
      retval); /* rename this func, '1' => open, '0' => closed */
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return retval ? 0 : 1;
}
/*
 * Simple accessor for tracked rise eventso index pin.
 * @return number of rising edges on idx pin.
 */
int CANTalon::GetNumberOfQuadIdxRises() const {
  int rises;
  CTR_Code status = m_impl->GetEncIndexRiseEvents(
      rises); /* rename this func, '1' => open, '0' => closed */
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return rises;
}
/*
 * @param rises integral value to set into index-rises register.  Great way to
 * zero the index count.
 */
void CANTalon::SetNumberOfQuadIdxRises(int rises) {
  CTR_Code status = m_impl->SetParam(
      CanTalonSRX::eEncIndexRiseEvents,
      rises); /* rename this func, '1' => open, '0' => closed */
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * TODO documentation (see CANJaguar.cpp)
 */
bool CANTalon::GetForwardLimitOK() const {
  int limSwit = 0;
  int softLim = 0;
  CTR_Code status = CTR_OKAY;
  status = m_impl->GetFault_ForSoftLim(softLim);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  status = m_impl->GetFault_ForLim(limSwit);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  /* If either fault is asserted, signal caller we are disabled (with false?) */
  return (softLim | limSwit) ? false : true;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
bool CANTalon::GetReverseLimitOK() const {
  int limSwit = 0;
  int softLim = 0;
  CTR_Code status = CTR_OKAY;
  status = m_impl->GetFault_RevSoftLim(softLim);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  status = m_impl->GetFault_RevLim(limSwit);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  /* If either fault is asserted, signal caller we are disabled (with false?) */
  return (softLim | limSwit) ? false : true;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
uint16_t CANTalon::GetFaults() const {
  uint16_t retval = 0;
  int val;
  CTR_Code status = CTR_OKAY;

  /* temperature */
  val = 0;
  status = m_impl->GetFault_OverTemp(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kTemperatureFault : 0;

  /* voltage */
  val = 0;
  status = m_impl->GetFault_UnderVoltage(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kBusVoltageFault : 0;

  /* fwd-limit-switch */
  val = 0;
  status = m_impl->GetFault_ForLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kFwdLimitSwitch : 0;

  /* rev-limit-switch */
  val = 0;
  status = m_impl->GetFault_RevLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kRevLimitSwitch : 0;

  /* fwd-soft-limit */
  val = 0;
  status = m_impl->GetFault_ForSoftLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kFwdSoftLimit : 0;

  /* rev-soft-limit */
  val = 0;
  status = m_impl->GetFault_RevSoftLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kRevSoftLimit : 0;

  return retval;
}
uint16_t CANTalon::GetStickyFaults() const {
  uint16_t retval = 0;
  int val;
  CTR_Code status = CTR_OKAY;

  /* temperature */
  val = 0;
  status = m_impl->GetStckyFault_OverTemp(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kTemperatureFault : 0;

  /* voltage */
  val = 0;
  status = m_impl->GetStckyFault_UnderVoltage(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kBusVoltageFault : 0;

  /* fwd-limit-switch */
  val = 0;
  status = m_impl->GetStckyFault_ForLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kFwdLimitSwitch : 0;

  /* rev-limit-switch */
  val = 0;
  status = m_impl->GetStckyFault_RevLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kRevLimitSwitch : 0;

  /* fwd-soft-limit */
  val = 0;
  status = m_impl->GetStckyFault_ForSoftLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kFwdSoftLimit : 0;

  /* rev-soft-limit */
  val = 0;
  status = m_impl->GetStckyFault_RevSoftLim(val);
  if (status != CTR_OKAY)
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  retval |= (val) ? CANSpeedController::kRevSoftLimit : 0;

  return retval;
}
void CANTalon::ClearStickyFaults() {
  CTR_Code status = m_impl->ClearStickyFaults();
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the maximum voltage change rate.  This ramp rate is in affect regardless
 * of which control mode
 * the TALON is in.
 *
 * When in PercentVbus or Voltage output mode, the rate at which the voltage
 * changes can
 * be limited to reduce current spikes.  Set this to 0.0 to disable rate
 * limiting.
 *
 * @param rampRate The maximum rate of voltage change in Percent Voltage mode in
 * V/s.
 */
void CANTalon::SetVoltageRampRate(double rampRate) {
  /* Caller is expressing ramp as Voltage per sec, assuming 12V is full.
          Talon's throttle ramp is in dThrot/d10ms.  1023 is full fwd, -1023 is
     full rev. */
  double rampRatedThrotPer10ms = (rampRate * 1023.0 / 12.0) / 100;
  CTR_Code status = m_impl->SetRampThrottle((int)rampRatedThrotPer10ms);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
void CANTalon::SetVoltageCompensationRampRate(double rampRate) {
  /* when in voltage compensation mode, the voltage compensation rate
    directly caps the change in target voltage */
  CTR_Code status = CTR_OKAY;
  status = m_impl->SetVoltageCompensationRate(rampRate / 1000);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Sets a voltage change rate that applies only when a close loop contorl mode
 * is enabled.
 * This allows close loop specific ramp behavior.
 *
 * @param rampRate The maximum rate of voltage change in Percent Voltage mode in
 * V/s.
 */
void CANTalon::SetCloseLoopRampRate(double rampRate) {
  CTR_Code status = m_impl->SetCloseLoopRampRate(
      m_profile, rampRate * 1023.0 / 12.0 / 1000.0);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * @return The version of the firmware running on the Talon
 */
uint32_t CANTalon::GetFirmwareVersion() const {
  int firmwareVersion;
  CTR_Code status = m_impl->RequestParam(CanTalonSRX::eFirmVers);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs);
  status =
      m_impl->GetParamResponseInt32(CanTalonSRX::eFirmVers, firmwareVersion);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }

  /* only sent once on boot */
  // CTR_Code status = m_impl->GetFirmVers(firmwareVersion);
  // if (status != CTR_OKAY) {
  //  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  //}

  return firmwareVersion;
}
/**
 * @return The accumulator for I gain.
 */
int CANTalon::GetIaccum() const {
  CTR_Code status = m_impl->RequestParam(CanTalonSRX::ePidIaccum);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  usleep(kDelayForSolicitedSignalsUs); /* small yield for getting response */
  int iaccum;
  status = m_impl->GetParamResponseInt32(CanTalonSRX::ePidIaccum, iaccum);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return iaccum;
}
/**
 * Clear the accumulator for I gain.
 */
void CANTalon::ClearIaccum() {
  CTR_Code status = m_impl->SetParam(CanTalonSRX::ePidIaccum, 0);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigNeutralMode(NeutralMode mode) {
  CTR_Code status = CTR_OKAY;
  switch (mode) {
    default:
    case kNeutralMode_Jumper: /* use default setting in flash based on
                                 webdash/BrakeCal button selection */
      status = m_impl->SetOverrideBrakeType(
          CanTalonSRX::kBrakeOverride_UseDefaultsFromFlash);
      break;
    case kNeutralMode_Brake:
      status = m_impl->SetOverrideBrakeType(
          CanTalonSRX::kBrakeOverride_OverrideBrake);
      break;
    case kNeutralMode_Coast:
      status = m_impl->SetOverrideBrakeType(
          CanTalonSRX::kBrakeOverride_OverrideCoast);
      break;
  }
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * @return nonzero if brake is enabled during neutral.  Zero if coast is enabled
 * during neutral.
 */
int CANTalon::GetBrakeEnableDuringNeutral() const {
  int brakeEn = 0;
  CTR_Code status = m_impl->GetBrakeIsEnabled(brakeEn);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return brakeEn;
}
/**
 * Configure how many codes per revolution are generated by your encoder.
 *
 * @param codesPerRev The number of counts per revolution.
 */
void CANTalon::ConfigEncoderCodesPerRev(uint16_t codesPerRev) {
  /* first save the scalar so that all getters/setter work as the user expects */
  m_codesPerRev = codesPerRev;
  /* next send the scalar to the Talon over CAN.  This is so that the Talon can report
    it to whoever needs it, like the webdash.  Don't bother checking the return,
    this is only for instrumentation and is not necessary for Talon functionality. */
  (void)m_impl->SetParam(CanTalonSRX::eNumberEncoderCPR, m_codesPerRev);
}

/**
 * Configure the number of turns on the potentiometer.
 *
 * @param turns The number of turns of the potentiometer.
 */
void CANTalon::ConfigPotentiometerTurns(uint16_t turns) {
  /* first save the scalar so that all getters/setter work as the user expects */
  m_numPotTurns = turns;
  /* next send the scalar to the Talon over CAN.  This is so that the Talon can report
    it to whoever needs it, like the webdash.  Don't bother checking the return,
    this is only for instrumentation and is not necessary for Talon functionality. */
  (void)m_impl->SetParam(CanTalonSRX::eNumberPotTurns, m_numPotTurns);
}

/**
 * @deprecated not implemented
 */
void CANTalon::ConfigSoftPositionLimits(double forwardLimitPosition,
                                        double reverseLimitPosition) {
  ConfigLimitMode(kLimitMode_SoftPositionLimits);
  ConfigForwardLimit(forwardLimitPosition);
  ConfigReverseLimit(reverseLimitPosition);
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::DisableSoftPositionLimits() {
  ConfigLimitMode(kLimitMode_SwitchInputsOnly);
}

/**
 * Overrides the forward and reverse limit switch enables.
 * Unlike ConfigLimitMode, this function allows individual control of forward and
 * reverse limit switch enables.
 * Unlike ConfigLimitMode, this function does not affect the soft-limit features of Talon SRX.
 * @see ConfigLimitMode()
 */
void CANTalon::ConfigLimitSwitchOverrides(bool bForwardLimitSwitchEn, bool bReverseLimitSwitchEn) {
  CTR_Code status = CTR_OKAY;
  int fwdRevEnable;
  /* chose correct signal value based on caller's requests enables */
  if(!bForwardLimitSwitchEn) {
    /* caller wants Forward Limit Switch OFF */
    if(!bReverseLimitSwitchEn) {
      /* caller wants both OFF */
      fwdRevEnable = CanTalonSRX::kLimitSwitchOverride_DisableFwd_DisableRev;
    } else {
      /* caller Forward OFF and Reverse ON */
      fwdRevEnable = CanTalonSRX::kLimitSwitchOverride_DisableFwd_EnableRev;
    }
  } else {
    /* caller wants Forward Limit Switch ON */
    if(!bReverseLimitSwitchEn) {
      /* caller wants Forward ON and Reverse OFF */
      fwdRevEnable = CanTalonSRX::kLimitSwitchOverride_EnableFwd_DisableRev;
    } else {
      /* caller wants both ON */
      fwdRevEnable = CanTalonSRX::kLimitSwitchOverride_EnableFwd_EnableRev;
    }
  }
  /* update signal and error check code */
  status = m_impl->SetOverrideLimitSwitchEn(fwdRevEnable);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Configures the soft limit enable (wear leveled persistent memory).
 * Also sets the limit switch overrides.
 */
void CANTalon::ConfigLimitMode(LimitMode mode) {
  CTR_Code status = CTR_OKAY;
  switch (mode) {
    case kLimitMode_SwitchInputsOnly: /** Only use switches for limits */
      /* turn OFF both limits. SRX has individual enables and polarity for each
       * limit switch.*/
      status = m_impl->SetForwardSoftEnable(false);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      status = m_impl->SetReverseSoftEnable(false);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      /* override enable the limit switches, this circumvents the webdash */
      status = m_impl->SetOverrideLimitSwitchEn(
          CanTalonSRX::kLimitSwitchOverride_EnableFwd_EnableRev);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      break;
    case kLimitMode_SoftPositionLimits: /** Use both switches and soft limits */
      /* turn on both limits. SRX has individual enables and polarity for each
       * limit switch.*/
      status = m_impl->SetForwardSoftEnable(true);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      status = m_impl->SetReverseSoftEnable(true);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      /* override enable the limit switches, this circumvents the webdash */
      status = m_impl->SetOverrideLimitSwitchEn(
          CanTalonSRX::kLimitSwitchOverride_EnableFwd_EnableRev);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      break;

    case kLimitMode_SrxDisableSwitchInputs: /** disable both limit switches and
                                               soft limits */
      /* turn on both limits. SRX has individual enables and polarity for each
       * limit switch.*/
      status = m_impl->SetForwardSoftEnable(false);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      status = m_impl->SetReverseSoftEnable(false);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      /* override enable the limit switches, this circumvents the webdash */
      status = m_impl->SetOverrideLimitSwitchEn(
          CanTalonSRX::kLimitSwitchOverride_DisableFwd_DisableRev);
      if (status != CTR_OKAY) {
        wpi_setErrorWithContext(status, getHALErrorMessage(status));
      }
      break;
  }
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigForwardLimit(double forwardLimitPosition) {
  CTR_Code status = CTR_OKAY;
  int32_t nativeLimitPos = ScaleRotationsToNativeUnits(m_feedbackDevice, forwardLimitPosition);
  status = m_impl->SetForwardSoftLimit(nativeLimitPos);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Set the Forward Soft Limit Enable.
 * This is the same setting that is in the Web-Based Configuration.
 * @param bForwardSoftLimitEn true to enable Soft limit, false to disable.
 */
void CANTalon::ConfigForwardSoftLimitEnable(bool bForwardSoftLimitEn) {
  CTR_Code status = CTR_OKAY;
  status = m_impl->SetForwardSoftEnable(bForwardSoftLimitEn);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Set the Reverse Soft Limit Enable.
 * This is the same setting that is in the Web-Based Configuration.
 * @param bReverseSoftLimitEn true to enable Soft limit, false to disable.
 */
void CANTalon::ConfigReverseSoftLimitEnable(bool bReverseSoftLimitEn) {
  CTR_Code status = CTR_OKAY;
  status = m_impl->SetReverseSoftEnable(bReverseSoftLimitEn);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Change the fwd limit switch setting to normally open or closed.
 * Talon will disable momentarilly if the Talon's current setting
 * is dissimilar to the caller's requested setting.
 *
 * Since Talon saves setting to flash this should only affect
 * a given Talon initially during robot install.
 *
 * @param normallyOpen true for normally open.  false for normally closed.
 */
void CANTalon::ConfigFwdLimitSwitchNormallyOpen(bool normallyOpen) {
  CTR_Code status =
      m_impl->SetParam(CanTalonSRX::eOnBoot_LimitSwitch_Forward_NormallyClosed,
                       normallyOpen ? 0 : 1);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Change the rev limit switch setting to normally open or closed.
 * Talon will disable momentarilly if the Talon's current setting
 * is dissimilar to the caller's requested setting.
 *
 * Since Talon saves setting to flash this should only affect
 * a given Talon initially during robot install.
 *
 * @param normallyOpen true for normally open.  false for normally closed.
 */
void CANTalon::ConfigRevLimitSwitchNormallyOpen(bool normallyOpen) {
  CTR_Code status =
      m_impl->SetParam(CanTalonSRX::eOnBoot_LimitSwitch_Reverse_NormallyClosed,
                       normallyOpen ? 0 : 1);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigReverseLimit(double reverseLimitPosition) {
  CTR_Code status = CTR_OKAY;
  int32_t nativeLimitPos = ScaleRotationsToNativeUnits(m_feedbackDevice, reverseLimitPosition);
  status = m_impl->SetReverseSoftLimit(nativeLimitPos);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigMaxOutputVoltage(double voltage) {
  /* config peak throttle when in closed-loop mode in the fwd and rev direction. */
  ConfigPeakOutputVoltage(voltage, -voltage);
}
void CANTalon::ConfigPeakOutputVoltage(double forwardVoltage,double reverseVoltage) {
  /* bounds checking */
  if (forwardVoltage > 12)
    forwardVoltage = 12;
  else if (forwardVoltage < 0)
    forwardVoltage = 0;
  if (reverseVoltage > 0)
    reverseVoltage = 0;
  else if (reverseVoltage < -12)
    reverseVoltage = -12;
  /* config calls */
  ConfigSetParameter(CanTalonSRX::ePeakPosOutput, 1023 * forwardVoltage / 12.0);
  ConfigSetParameter(CanTalonSRX::ePeakNegOutput, 1023 * reverseVoltage / 12.0);
}
void CANTalon::ConfigNominalOutputVoltage(double forwardVoltage,double reverseVoltage) {
  /* bounds checking */
  if (forwardVoltage > 12)
    forwardVoltage = 12;
  else if (forwardVoltage < 0)
    forwardVoltage = 0;
  if (reverseVoltage > 0)
    reverseVoltage = 0;
  else if (reverseVoltage < -12)
    reverseVoltage = -12;
  /* config calls */
  ConfigSetParameter(CanTalonSRX::eNominalPosOutput,1023*forwardVoltage/12.0);
  ConfigSetParameter(CanTalonSRX::eNominalNegOutput,1023*reverseVoltage/12.0);
}
/**
 * General set frame.  Since the parameter is a general integral type, this can
 * be used for testing future features.
 */
void CANTalon::ConfigSetParameter(uint32_t paramEnum, double value) {
  CTR_Code status;
  /* config peak throttle when in closed-loop mode in the positive direction. */
  status = m_impl->SetParam((CanTalonSRX::param_t)paramEnum,value);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * General get frame.  Since the parameter is a general integral type, this can
 * be used for testing future features.
 */
bool CANTalon::GetParameter(uint32_t paramEnum, double & dvalue) const {
  bool retval = true;
  /* send the request frame */
  CTR_Code status = m_impl->RequestParam((CanTalonSRX::param_t)paramEnum);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    retval = false;
  }
  /* small yield for getting response */
  usleep(kDelayForSolicitedSignalsUs);
  /* get the last received update */
  status = m_impl->GetParamResponse((CanTalonSRX::param_t)paramEnum, dvalue);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    retval = false;
  }
  return retval;
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::ConfigFaultTime(float faultTime) {
  /* SRX does not have fault time.  SRX motor drive is only disabled for soft
   * limits and limit-switch faults. */
  wpi_setWPIErrorWithContext(IncompatibleMode, "Fault Time not supported.");
}

/**
 * Fixup the sendMode so Set() serializes the correct demand value.
 * Also fills the modeSelecet in the control frame to disabled.
 * @param mode Control mode to ultimately enter once user calls Set().
 * @see Set()
 */
void CANTalon::ApplyControlMode(CANSpeedController::ControlMode mode) {
  m_controlMode = mode;
  HALReport(HALUsageReporting::kResourceType_CANTalonSRX, m_deviceNumber + 1,
            mode);
  switch (mode) {
    case kPercentVbus:
      m_sendMode = kThrottle;
      break;
    case kCurrent:
      m_sendMode = kCurrentMode;
      break;
    case kSpeed:
      m_sendMode = kSpeedMode;
      break;
    case kPosition:
      m_sendMode = kPositionMode;
      break;
    case kVoltage:
      m_sendMode = kVoltageMode;
      break;
    case kFollower:
      m_sendMode = kFollowerMode;
      break;
    case kMotionProfile:
      m_sendMode = kMotionProfileMode;
      break;
  }
  // Keep the talon disabled until Set() is called.
  CTR_Code status = m_impl->SetModeSelect((int)kDisabled);
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * TODO documentation (see CANJaguar.cpp)
 */
void CANTalon::SetControlMode(CANSpeedController::ControlMode mode) {
  if (m_controlMode == mode) {
    /* we already are in this mode, don't perform disable workaround */
  } else {
    ApplyControlMode(mode);
  }
}

/**
 * TODO documentation (see CANJaguar.cpp)
 */
CANSpeedController::ControlMode CANTalon::GetControlMode() const {
  return m_controlMode;
}

void CANTalon::SetExpiration(float timeout) {
  m_safetyHelper->SetExpiration(timeout);
}

float CANTalon::GetExpiration() const {
  return m_safetyHelper->GetExpiration();
}

bool CANTalon::IsAlive() const { return m_safetyHelper->IsAlive(); }

bool CANTalon::IsSafetyEnabled() const {
  return m_safetyHelper->IsSafetyEnabled();
}

void CANTalon::SetSafetyEnabled(bool enabled) {
  m_safetyHelper->SetSafetyEnabled(enabled);
}

void CANTalon::GetDescription(std::ostringstream& desc) const {
  desc << "CANTalon ID " <<  m_deviceNumber;
}
/**
 * @param devToLookup FeedbackDevice to lookup the scalar for.  Because Talon
 *            allows multiple sensors to be attached simultaneously, caller must
 *            specify which sensor to lookup.
 * @return    The number of native Talon units per rotation of the selected sensor.
 *            Zero if the necessary sensor information is not available.
 * @see ConfigEncoderCodesPerRev
 * @see ConfigPotentiometerTurns
 */
double CANTalon::GetNativeUnitsPerRotationScalar(FeedbackDevice devToLookup)const
{
  bool scalingAvail = false;
  CTR_Code status = CTR_OKAY;
  double retval = 0;
  switch (devToLookup) {
    case QuadEncoder:
    { /* When caller wants to lookup Quadrature, the QEI may be in 1x if the selected feedback is edge counter.
       * Additionally if the quadrature source is the CTRE Mag encoder, then the CPR is known.
       * This is nice in that the calling app does not require knowing the CPR at all.
       * So do both checks here.
       */
      int32_t qeiPulsePerCount = 4; /* default to 4x */
      switch (m_feedbackDevice) {
        case CtreMagEncoder_Relative:
        case CtreMagEncoder_Absolute:
          /* we assume the quadrature signal comes from the MagEnc,
            of which we know the CPR already */
          retval = kNativePwdUnitsPerRotation;
          scalingAvail = true;
          break;
        case EncRising: /* Talon's QEI is setup for 1x, so perform 1x math */
        case EncFalling:
          qeiPulsePerCount = 1;
          break;
        case QuadEncoder: /* Talon's QEI is 4x */
        default: /* pulse width and everything else, assume its regular quad use. */
          break;
      }
      if (scalingAvail) {
        /* already deduced the scalar above, we're done. */
      } else {
        /* we couldn't deduce the scalar just based on the selection */
        if (0 == m_codesPerRev) {
          /* caller has never set the CPR.  Most likely caller
            is just using engineering units so fall to the
            bottom of this func.*/
        } else {
          /* Talon expects PPR units */
          retval = qeiPulsePerCount * m_codesPerRev;
          scalingAvail = true;
        }
      }
    }  break;
    case EncRising:
    case EncFalling:
      if (0 == m_codesPerRev) {
        /* caller has never set the CPR.  Most likely caller
          is just using engineering units so fall to the
          bottom of this func.*/
      } else {
        /* Talon expects PPR units */
        retval = 1 * m_codesPerRev;
        scalingAvail = true;
      }
      break;
    case AnalogPot:
    case AnalogEncoder:
      if (0 == m_numPotTurns) {
        /* caller has never set the CPR.  Most likely caller
          is just using engineering units so fall to the
          bottom of this func.*/
      } else {
        retval = (double)kNativeAdcUnitsPerRotation / m_numPotTurns;
        scalingAvail = true;
      }
      break;
    case CtreMagEncoder_Relative:
    case CtreMagEncoder_Absolute:
    case PulseWidth:
      retval = kNativePwdUnitsPerRotation;
      scalingAvail = true;
      break;
  }
  /* handle any detected errors */
  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  /* if scaling information is not possible, signal caller
    by returning zero */
  if (false == scalingAvail)
    retval = 0;
  return retval;
}
/**
 * @param fullRotations   double precision value representing number of rotations of selected feedback sensor.
 *              If user has never called the config routine for the selected sensor, then the caller
 *              is likely passing rotations in engineering units already, in which case it is returned
 *              as is.
 *              @see ConfigPotentiometerTurns
 *              @see ConfigEncoderCodesPerRev
 * @return fullRotations in native engineering units of the Talon SRX firmware.
 */
int32_t CANTalon::ScaleRotationsToNativeUnits(FeedbackDevice devToLookup,double fullRotations)const
{
  /* first assume we don't have config info, prep the default return */
  int32_t retval = (int32_t)fullRotations;
  /* retrieve scaling info */
  double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
  /* apply scalar if its available */
  if (scalar > 0)
    retval = (int32_t)(fullRotations*scalar);
  return retval;
}
/**
 * @param rpm   double precision value representing number of rotations per minute of selected feedback sensor.
 *              If user has never called the config routine for the selected sensor, then the caller
 *              is likely passing rotations in engineering units already, in which case it is returned
 *              as is.
 *              @see ConfigPotentiometerTurns
 *              @see ConfigEncoderCodesPerRev
 * @return sensor velocity in native engineering units of the Talon SRX firmware.
 */
int32_t CANTalon::ScaleVelocityToNativeUnits(FeedbackDevice devToLookup,double rpm)const
{
  /* first assume we don't have config info, prep the default return */
  int32_t retval = (int32_t)rpm;
  /* retrieve scaling info */
  double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
  /* apply scalar if its available */
  if (scalar > 0)
    retval = (int32_t)(rpm * kMinutesPer100msUnit * scalar);
  return retval;
}
/**
 * @param nativePos   integral position of the feedback sensor in native Talon SRX units.
 *              If user has never called the config routine for the selected sensor, then the return
 *              will be in TALON SRX units as well to match the behavior in the 2015 season.
 *              @see ConfigPotentiometerTurns
 *              @see ConfigEncoderCodesPerRev
 * @return double precision number of rotations, unless config was never performed.
 */
double CANTalon::ScaleNativeUnitsToRotations(FeedbackDevice devToLookup,int32_t nativePos)const
{
  /* first assume we don't have config info, prep the default return */
  double retval = (double)nativePos;
  /* retrieve scaling info */
  double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
  /* apply scalar if its available */
  if (scalar > 0)
    retval = ((double)nativePos) / scalar;
  return retval;
}
/**
 * @param nativeVel   integral velocity of the feedback sensor in native Talon SRX units.
 *              If user has never called the config routine for the selected sensor, then the return
 *              will be in TALON SRX units as well to match the behavior in the 2015 season.
 *              @see ConfigPotentiometerTurns
 *              @see ConfigEncoderCodesPerRev
 * @return double precision of sensor velocity in RPM, unless config was never performed.
 */
double CANTalon::ScaleNativeUnitsToRpm(FeedbackDevice devToLookup, int32_t nativeVel)const
{
  /* first assume we don't have config info, prep the default return */
  double retval = (double)nativeVel;
  /* retrieve scaling info */
  double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
  /* apply scalar if its available */
  if (scalar > 0)
    retval = (double)(nativeVel) / (scalar*kMinutesPer100msUnit);
  return retval;
}

/**
 * Enables Talon SRX to automatically zero the Sensor Position whenever an
 * edge is detected on the index signal.
 * @param enable     boolean input, pass true to enable feature or false to disable.
 * @param risingEdge   boolean input, pass true to clear the position on rising edge,
 *          pass false to clear the position on falling edge.
 */
void CANTalon::EnableZeroSensorPositionOnIndex(bool enable, bool risingEdge)
{
  if (enable) {
    /* enable the feature, update the edge polarity first to ensure
      it is correct before the feature is enabled. */
    ConfigSetParameter(CanTalonSRX::eQuadIdxPolarity,risingEdge  ? 1 : 0);
    ConfigSetParameter(CanTalonSRX::eClearPositionOnIdx,1);
  } else {
    /* disable the feature first, then update the edge polarity. */
    ConfigSetParameter(CanTalonSRX::eClearPositionOnIdx,0);
    ConfigSetParameter(CanTalonSRX::eQuadIdxPolarity,risingEdge  ? 1 : 0);
  }
}

/**
 * Calling application can opt to speed up the handshaking between the robot API and the Talon to increase the
 * download rate of the Talon's Motion Profile.  Ideally the period should be no more than half the period
 * of a trajectory point.
 */
void CANTalon::ChangeMotionControlFramePeriod(int periodMs)
{
  m_impl->ChangeMotionControlFramePeriod(periodMs);
}

/**
 * Clear the buffered motion profile in both Talon RAM (bottom), and in the API (top).
 * Be sure to check GetMotionProfileStatus() to know when the buffer is actually cleared.
 */
void CANTalon::ClearMotionProfileTrajectories()
{
  m_impl->ClearMotionProfileTrajectories();
}

/**
 * Retrieve just the buffer count for the api-level (top) buffer.
 * This routine performs no CAN or data structure lookups, so its fast and ideal
 * if caller needs to quickly poll the progress of trajectory points being emptied
 * into Talon's RAM. Otherwise just use GetMotionProfileStatus.
 * @return number of trajectory points in the top buffer.
 */
int CANTalon::GetMotionProfileTopLevelBufferCount()
{
  return m_impl->GetMotionProfileTopLevelBufferCount();
}

/**
 * Push another trajectory point into the top level buffer (which is emptied into
 * the Talon's bottom buffer as room allows).
 * @param trajPt the trajectory point to insert into buffer.
 * @return true  if trajectory point push ok. CTR_BufferFull if buffer is full
 * due to kMotionProfileTopBufferCapacity.
 */
bool CANTalon::PushMotionProfileTrajectory(const TrajectoryPoint & trajPt)
{
  /* convert positiona and velocity to native units */
  int32_t targPos  = ScaleRotationsToNativeUnits(m_feedbackDevice, trajPt.position);
  int32_t targVel = ScaleVelocityToNativeUnits(m_feedbackDevice, trajPt.velocity);
  /* bounds check signals that require it */
  uint32_t profileSlotSelect = (trajPt.profileSlotSelect) ? 1 : 0;
  uint8_t timeDurMs = (trajPt.timeDurMs >= 255) ? 255 : trajPt.timeDurMs; /* cap time to 255ms */
  /* send it to the top level buffer */
  CTR_Code status = m_impl->PushMotionProfileTrajectory(targPos, targVel, profileSlotSelect, timeDurMs, trajPt.velocityOnly, trajPt.isLastPoint, trajPt.zeroPos);
  return (status == CTR_OKAY) ? true : false;
}
/**
 * @return true if api-level (top) buffer is full.
 */
bool CANTalon::IsMotionProfileTopLevelBufferFull()
{
  return m_impl->IsMotionProfileTopLevelBufferFull();
}

/**
 * This must be called periodically to funnel the trajectory points from the API's top level buffer to
 * the Talon's bottom level buffer.  Recommendation is to call this twice as fast as the executation rate of the motion profile.
 * So if MP is running with 20ms trajectory points, try calling this routine every 10ms.  All motion profile functions are thread-safe
 * through the use of a mutex, so there is no harm in having the caller utilize threading.
 */
void CANTalon::ProcessMotionProfileBuffer()
{
  m_impl->ProcessMotionProfileBuffer();
}

/**
 * Retrieve all status information.
 * Since this all comes from one CAN frame, its ideal to have one routine to retrieve the frame once and decode everything.
 * @param [out] motionProfileStatus contains all progress information on the currently running MP.
 */
void CANTalon::GetMotionProfileStatus(MotionProfileStatus & motionProfileStatus)
{
  uint32_t flags;
  uint32_t profileSlotSelect;
  int32_t targPos, targVel;
  uint32_t topBufferRem, topBufferCnt, btmBufferCnt;
  uint32_t outputEnable;
  /* retrieve all motion profile signals from status frame */
  CTR_Code status = m_impl->GetMotionProfileStatus(flags, profileSlotSelect, targPos, targVel, topBufferRem, topBufferCnt, btmBufferCnt, outputEnable);
  /* completely update the caller's structure */
  motionProfileStatus.topBufferRem = topBufferRem;
  motionProfileStatus.topBufferCnt = topBufferCnt;
  motionProfileStatus.btmBufferCnt = btmBufferCnt;
  motionProfileStatus.hasUnderrun =              (flags & CanTalonSRX::kMotionProfileFlag_HasUnderrun)     ? true :false;
  motionProfileStatus.isUnderrun  =              (flags & CanTalonSRX::kMotionProfileFlag_IsUnderrun)      ? true :false;
  motionProfileStatus.activePointValid =         (flags & CanTalonSRX::kMotionProfileFlag_ActTraj_IsValid) ? true :false;
  motionProfileStatus.activePoint.isLastPoint =  (flags & CanTalonSRX::kMotionProfileFlag_ActTraj_IsLast)  ? true :false;
  motionProfileStatus.activePoint.velocityOnly = (flags & CanTalonSRX::kMotionProfileFlag_ActTraj_VelOnly) ? true :false;
  motionProfileStatus.activePoint.position = ScaleNativeUnitsToRotations(m_feedbackDevice, targPos);
  motionProfileStatus.activePoint.velocity = ScaleNativeUnitsToRpm(m_feedbackDevice, targVel);
  motionProfileStatus.activePoint.profileSlotSelect = profileSlotSelect;
  switch(outputEnable){
    case CanTalonSRX::kMotionProf_Disabled:
      motionProfileStatus.outputEnable = SetValueMotionProfileDisable;
    break;
    case CanTalonSRX::kMotionProf_Enable:
      motionProfileStatus.outputEnable = SetValueMotionProfileEnable;
      break;
    case CanTalonSRX::kMotionProf_Hold:
      motionProfileStatus.outputEnable = SetValueMotionProfileHold;
      break;
    default:
      motionProfileStatus.outputEnable = SetValueMotionProfileDisable;
      break;
  }
  motionProfileStatus.activePoint.zeroPos = false; /* this signal is only used sending pts to Talon */
  motionProfileStatus.activePoint.timeDurMs = 0;   /* this signal is only used sending pts to Talon */

  if (status != CTR_OKAY) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}
/**
 * Clear the hasUnderrun flag in Talon's Motion Profile Executer when MPE is ready for another point,
 * but the low level buffer is empty.
 *
 * Once the Motion Profile Executer sets the hasUnderrun flag, it stays set until
 * Robot Application clears it with this routine, which ensures Robot Application
 * gets a chance to instrument or react.  Caller could also check the isUnderrun flag
 * which automatically clears when fault condition is removed.
 */
void CANTalon::ClearMotionProfileHasUnderrun()
{
  ConfigSetParameter(CanTalonSRX::eMotionProfileHasUnderrunErr, 0);
}
/**
* Common interface for inverting direction of a speed controller.
* Only works in PercentVbus, speed, and Voltage modes.
* @param isInverted The state of inversion, true is inverted.
*/
void CANTalon::SetInverted(bool isInverted) { m_isInverted = isInverted; }

/**
 * Common interface for the inverting direction of a speed controller.
 *
 * @return isInverted The state of inversion, true is inverted.
 *
 */
bool CANTalon::GetInverted() const { return m_isInverted; }

/**
 * Common interface for stopping the motor until the next Set() call
 * Part of the MotorSafety interface
 *
 * @deprecated Call Disable instead.
*/
void CANTalon::StopMotor() {
	Disable();
	m_stopped = true;
}

void CANTalon::ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew) {
  if(key == "Mode" && value->IsDouble()) SetControlMode(static_cast<CANSpeedController::ControlMode>(value->GetDouble()));
  if(key == "p" && value->IsDouble()) SetP(value->GetDouble());
  if(key == "i" && value->IsDouble()) SetI(value->GetDouble());
  if(key == "d" && value->IsDouble()) SetD(value->GetDouble());
  if(key == "f" && value->IsDouble()) SetF(value->GetDouble());
  if(key == "Enabled" && value->IsBoolean()) {
      if (value->GetBoolean()) {
        Enable();
      } else {
        Disable();
      }
  }
  if(key == "Value" && value->IsDouble()) Set(value->GetDouble());
}

bool CANTalon::IsModePID(CANSpeedController::ControlMode mode) const {
  return mode == kCurrent || mode == kSpeed || mode == kPosition;
}

void CANTalon::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutString("~TYPE~", "CANSpeedController");
    m_table->PutString("Type", "CANTalon");
    m_table->PutNumber("Mode", m_controlMode);
    m_table->PutNumber("p", GetP());
    m_table->PutNumber("i", GetI());
    m_table->PutNumber("d", GetD());
    m_table->PutNumber("f", GetF());
    m_table->PutBoolean("Enabled", IsControlEnabled());
    m_table->PutNumber("Value", Get());
  }
}

void CANTalon::StartLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->AddTableListener(this, true);
  }
}

void CANTalon::StopLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string CANTalon::GetSmartDashboardType() const {
  return "CANSpeedController";
}

void CANTalon::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> CANTalon::GetTable() const { return m_table; }
