/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PWM.h"

#include "Resource.h"
#include "Utility.h"
#include "WPIErrors.h"
#include "HAL/HAL.hpp"

#include <sstream>

constexpr float PWM::kDefaultPwmPeriod;
constexpr float PWM::kDefaultPwmCenter;
const int32_t PWM::kDefaultPwmStepsDown;
const int32_t PWM::kPwmDisabled;

/**
 * Allocate a PWM given a channel number.
 *
 * Checks channel value range and allocates the appropriate channel.
 * The allocation is only done to help users ensure that they don't double
 * assign channels.
 * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the MXP
 * port
 */
PWM::PWM(uint32_t channel) {
  std::stringstream buf;

  if (!CheckPWMChannel(channel)) {
    buf << "PWM Channel " << channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    return;
  }

  int32_t status = 0;
  allocatePWMChannel(m_pwm_ports[channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  m_channel = channel;

  setPWMRaw(m_pwm_ports[m_channel], kPwmDisabled, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  m_eliminateDeadband = false;

  HALReport(HALUsageReporting::kResourceType_PWM, channel);
}

/**
 * Free the PWM channel.
 *
 * Free the resource associated with the PWM channel and set the value to 0.
 */
PWM::~PWM() {
  int32_t status = 0;

  setPWMRaw(m_pwm_ports[m_channel], kPwmDisabled, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  freePWMChannel(m_pwm_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Optionally eliminate the deadband from a speed controller.
 * @param eliminateDeadband If true, set the motor curve on the Jaguar to
 * eliminate
 * the deadband in the middle of the range. Otherwise, keep the full range
 * without
 * modifying any values.
 */
void PWM::EnableDeadbandElimination(bool eliminateDeadband) {
  if (StatusIsFatal()) return;
  m_eliminateDeadband = eliminateDeadband;
  setPWMEliminateDeadband(m_pwm_ports[m_channel], eliminateDeadband);
}

/**
 * Set the bounds on the PWM values.
 * This sets the bounds on the PWM values for a particular each type of
 * controller. The values
 * determine the upper and lower speeds as well as the deadband bracket.
 * @param max The Minimum pwm value
 * @param deadbandMax The high end of the deadband range
 * @param center The center speed (off)
 * @param deadbandMin The low end of the deadband range
 * @param min The minimum pwm value
 */
void PWM::SetBounds(int32_t max, int32_t deadbandMax, int32_t center,
                    int32_t deadbandMin, int32_t min) {
  if (StatusIsFatal()) return;
  m_maxPwm = max;
  m_deadbandMaxPwm = deadbandMax;
  m_centerPwm = center;
  m_deadbandMinPwm = deadbandMin;
  m_minPwm = min;
  setPWMBounds(m_pwm_ports[m_channel], max, deadbandMax, center, 
               deadbandMin, min);
}

/**
 * Set the bounds on the PWM pulse widths.
 * This sets the bounds on the PWM values for a particular type of controller.
 * The values
 * determine the upper and lower speeds as well as the deadband bracket.
 * @param max The max PWM pulse width in ms
 * @param deadbandMax The high end of the deadband range pulse width in ms
 * @param center The center (off) pulse width in ms
 * @param deadbandMin The low end of the deadband pulse width in ms
 * @param min The minimum pulse width in ms
 */
void PWM::SetBounds(double max, double deadbandMax, double center,
                    double deadbandMin, double min) {
  // calculate the loop time in milliseconds
  int32_t status = 0;
  double loopTime =
      getLoopTiming(&status) / (kSystemClockTicksPerMicrosecond * 1e3);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  if (StatusIsFatal()) return;

  m_maxPwm = (int32_t)((max - kDefaultPwmCenter) / loopTime +
                       kDefaultPwmStepsDown - 1);
  m_deadbandMaxPwm = (int32_t)((deadbandMax - kDefaultPwmCenter) / loopTime +
                               kDefaultPwmStepsDown - 1);
  m_centerPwm = (int32_t)((center - kDefaultPwmCenter) / loopTime +
                          kDefaultPwmStepsDown - 1);
  m_deadbandMinPwm = (int32_t)((deadbandMin - kDefaultPwmCenter) / loopTime +
                               kDefaultPwmStepsDown - 1);
  m_minPwm = (int32_t)((min - kDefaultPwmCenter) / loopTime +
                       kDefaultPwmStepsDown - 1);
  status = 0;
  setPWMBoundsDouble(m_pwm_ports[m_channel], max, deadbandMax, center, 
                     deadbandMin, min, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the PWM value based on a position.
 *
 * This is intended to be used by servos.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @param pos The position to set the servo between 0.0 and 1.0.
 */
void PWM::SetPosition(float pos) {
  if (StatusIsFatal()) return;
  
  // clamp between 0 and 1.
  if (pos < 0.0) {
    pos = 0.0;
  } else if (pos > 1.0) {
    pos = 1.0;
  }
  // scale to -1 to 1
  pos = pos * 2;
  pos = pos - 1;
  
  int32_t status = 0;
  setPWM(m_pwm_ports[m_channel], pos, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the PWM value in terms of a position.
 *
 * This is intended to be used by servos.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @return The position the servo is set to between 0.0 and 1.0.
 */
float PWM::GetPosition() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  float value = getPWM(m_pwm_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  if (StatusIsFatal()) return 0.0;
  
  // value returned is scaled -1 to 1.
  // scale from 0 to -1 instead.
  value = value + 1;
  value = value / 2.0f;
  
  return value;
}

/**
 * Set the PWM value based on a speed.
 *
 * This is intended to be used by speed controllers.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinPositivePwm() called.
 * @pre SetCenterPwm() called.
 * @pre SetMaxNegativePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @param speed The speed to set the speed controller between -1.0 and 1.0.
 */
void PWM::SetSpeed(float speed) {
  if (StatusIsFatal()) return;
  // clamp speed to be in the range 1.0 >= speed >= -1.0
  if (speed < -1.0) {
    speed = -1.0;
  } else if (speed > 1.0) {
    speed = 1.0;
  }
  
  int32_t status = 0;
  setPWM(m_pwm_ports[m_channel], speed, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the PWM value in terms of speed.
 *
 * This is intended to be used by speed controllers.
 *
 * @pre SetMaxPositivePwm() called.
 * @pre SetMinPositivePwm() called.
 * @pre SetMaxNegativePwm() called.
 * @pre SetMinNegativePwm() called.
 *
 * @return The most recently set speed between -1.0 and 1.0.
 */
float PWM::GetSpeed() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  float value = getPWM(m_pwm_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * Set the PWM value directly to the hardware.
 *
 * Write a raw value to a PWM channel.
 *
 * @param value Raw PWM value.
 */
void PWM::SetRaw(unsigned short value) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  setPWMRaw(m_pwm_ports[m_channel], value, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the PWM value directly from the hardware.
 *
 * Read a raw value from a PWM channel.
 *
 * @return Raw PWM control value.
 */
unsigned short PWM::GetRaw() const {
  if (StatusIsFatal()) return 0;

  int32_t status = 0;
  unsigned short value = getPWMRaw(m_pwm_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  return value;
}

/**
 * Slow down the PWM signal for old devices.
 *
 * @param mult The period multiplier to apply to this channel
 */
void PWM::SetPeriodMultiplier(PeriodMultiplier mult) {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  switch (mult) {
    case kPeriodMultiplier_4X:
      setPWMPeriodScale(m_pwm_ports[m_channel], 3,
                        &status);  // Squelch 3 out of 4 outputs
      break;
    case kPeriodMultiplier_2X:
      setPWMPeriodScale(m_pwm_ports[m_channel], 1,
                        &status);  // Squelch 1 out of 2 outputs
      break;
    case kPeriodMultiplier_1X:
      setPWMPeriodScale(m_pwm_ports[m_channel], 0,
                        &status);  // Don't squelch any outputs
      break;
    default:
      wpi_assert(false);
  }

  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

void PWM::SetZeroLatch() {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  latchPWMZero(m_pwm_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

void PWM::ValueChanged(ITable* source, llvm::StringRef key,
                       std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsDouble()) return;
  SetSpeed(value->GetDouble());
}

void PWM::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetSpeed());
  }
}

void PWM::StartLiveWindowMode() {
  SetSpeed(0);
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void PWM::StopLiveWindowMode() {
  SetSpeed(0);
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string PWM::GetSmartDashboardType() const { return "Speed Controller"; }

void PWM::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> PWM::GetTable() const { return m_table; }
