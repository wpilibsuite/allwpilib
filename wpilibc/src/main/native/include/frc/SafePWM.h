/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/raw_ostream.h>

#include "frc/MotorSafety.h"
#include "frc/MotorSafetyHelper.h"
#include "frc/PWM.h"

namespace frc {

/**
 * A safe version of the PWM class.
 *
 * It is safe because it implements the MotorSafety interface that provides
 * timeouts in the event that the motor value is not updated before the
 * expiration time. This delegates the actual work to a MotorSafetyHelper
 * object that is used for all objects that implement MotorSafety.
 */
class SafePWM : public PWM, public MotorSafety {
 public:
  /**
   * Constructor for a SafePWM object taking a channel number.
   *
   * @param channel The PWM channel number 0-9 are on-board, 10-19 are on the
   *                MXP port
   */
  explicit SafePWM(int channel);

  virtual ~SafePWM() = default;

  SafePWM(SafePWM&&) = default;
  SafePWM& operator=(SafePWM&&) = default;

  /**
   * Set the expiration time for the PWM object.
   *
   * @param timeout The timeout (in seconds) for this motor object
   */
  void SetExpiration(double timeout);

  /**
   * Return the expiration time for the PWM object.
   *
   * @returns The expiration time value.
   */
  double GetExpiration() const;

  /**
   * Check if the PWM object is currently alive or stopped due to a timeout.
   *
   * @return a bool value that is true if the motor has NOT timed out and should
   *         still be running.
   */
  bool IsAlive() const;

  /**
   * Stop the motor associated with this PWM object.
   *
   * This is called by the MotorSafetyHelper object when it has a timeout for
   * this PWM and needs to stop it from running.
   */
  void StopMotor();

  /**
   * Enable/disable motor safety for this device.
   *
   * Turn on and off the motor safety option for this PWM object.
   *
   * @param enabled True if motor safety is enforced for this object
   */
  void SetSafetyEnabled(bool enabled);

  /**
   * Check if motor safety is enabled for this object.
   *
   * @returns True if motor safety is enforced for this object
   */
  bool IsSafetyEnabled() const;

  void GetDescription(wpi::raw_ostream& desc) const;

  /**
   * Feed the MotorSafety timer when setting the speed.
   *
   * This method is called by the subclass motor whenever it updates its speed,
   * thereby reseting the timeout value.
   *
   * @param speed Value to pass to the PWM class
   */
  virtual void SetSpeed(double speed);

 private:
  std::unique_ptr<MotorSafetyHelper> m_safetyHelper;
};

}  // namespace frc
