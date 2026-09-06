/*
* Copyright (C) Cross The Road Electronics.  All rights reserved.
* License information can be found in CTRE_LICENSE.txt
* For support and suggestions contact support@ctr-electronics.com or file
* an issue tracker at https://github.com/CrossTheRoadElec/Phoenix-Releases
*/

#include "frc/motorcontrol/PWMTalonFXS.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

void frc::PWMTalonFXS::Set(double speed) {
  /* timer is running means we are configuring */
  if (!_timer.IsRunning()) {
    /* timer not running, we are not configurating */
    if (!DriverStation::IsEnabled() || _configs.empty()) {
      /* turn off timer */
      _timer.Stop();

      /* do what the base class normally does */
      PWMMotorController::Set(speed);
    } else {
      /* start timer */
      _timer.Restart();

      /* send PWM */
      m_pwm.SetPulseTime(_configs.front());
    }
  } else {
    /* timer running, we are applying a config */
    if (!DriverStation::IsEnabled()) {
      /* turn off timer, abandon the pulse*/
      _timer.Stop();

      /* do what the base class normally does */
      PWMMotorController::Set(speed);
    } else if (!IsTmrExpired()) {
      /* Still waiting on config pulses to finish */
    } else {
      /* tmr period expired */

      /* remove the config we've applied */
      _configs.erase(_configs.begin());

      /* next steps */
      if (_configs.empty()) {
        /* turn off timer */
        _timer.Stop();

        /* do what the base class normally does */
        PWMMotorController::Set(speed);
      } else {
        /* start timer */
        _timer.Restart();

        /* send PWM */
        m_pwm.SetPulseTime(_configs.front());
      }
    }
  }
}

bool frc::PWMTalonFXS::SetNeutralMode(bool bIsBrake)
{
  if (_configs.size() > 10) {
    return false;
  }
  _configs.push_back(bIsBrake ? units::microsecond_t{4000} : units::microsecond_t{3500});

  if (DriverStation::IsEnabled()) {
    Set(0);
  }
  return true;
}

bool frc::PWMTalonFXS::SetMotorArrangement(MotorArrangement motorArrangement)
{
  if (_configs.size() > 10) {
    return false;
  }

  auto microseconds = units::microsecond_t{0};
  switch (motorArrangement) {
  case MotorArrangement::Minion_JST:
    microseconds = units::microsecond_t{3000};
    break;
  case MotorArrangement::NEO_JST:
    microseconds = units::microsecond_t{3100};
    break;
  case MotorArrangement::NEO550_JST:
    microseconds = units::microsecond_t{3200};
    break;
  case MotorArrangement::VORTEX_JST:
    microseconds = units::microsecond_t{3300};
    break;
  case MotorArrangement::Brushed_DC:
    microseconds = units::microsecond_t{3700};
    break;
  default:
    return false;
  }

  _configs.push_back(microseconds);

  if (DriverStation::IsEnabled()) {
    Set(0);
  }
  return true;
}

bool frc::PWMTalonFXS::IsTmrExpired()
{
    return _timer.Get() > 0.1_s;
}

PWMTalonFXS::PWMTalonFXS(int channel) : PWMMotorController("PWMTalonFXS", channel)
{
    m_pwm.SetBounds(2.004_ms, 1.52_ms, 1.5_ms, 1.48_ms, 0.997_ms);
    m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_1X);
    m_pwm.SetSpeed(0.0);
    m_pwm.SetZeroLatch();

    HAL_Report(HALUsageReporting::kResourceType_CTRE_future12, GetChannel() + 1);
}
