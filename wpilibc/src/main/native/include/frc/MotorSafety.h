// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "frc/ErrorBase.h"
#include "frc/Timer.h"

namespace frc {

/**
 * This base class runs a watchdog timer and calls the subclass's StopMotor()
 * function if the timeout expires.
 *
 * The subclass should call Feed() whenever the motor value is updated.
 */
class MotorSafety : public ErrorBase {
 public:
  MotorSafety();
  ~MotorSafety() override;

  MotorSafety(MotorSafety&& rhs);
  MotorSafety& operator=(MotorSafety&& rhs);

  /**
   * Feed the motor safety object.
   *
   * Resets the timer on this object that is used to do the timeouts.
   */
  void Feed();

  /**
   * Set the expiration time for the corresponding motor safety object.
   *
   * @param expirationTime The timeout value in seconds.
   */
  void SetExpiration(double expirationTime);

  /**
   * Retrieve the timeout value for the corresponding motor safety object.
   *
   * @return the timeout value in seconds.
   */
  double GetExpiration() const;

  /**
   * Determine if the motor is still operating or has timed out.
   *
   * @return true if the motor is still operating normally and hasn't timed out.
   */
  bool IsAlive() const;

  /**
   * Enable/disable motor safety for this device.
   *
   * Turn on and off the motor safety option for this PWM object.
   *
   * @param enabled True if motor safety is enforced for this object.
   */
  void SetSafetyEnabled(bool enabled);

  /**
   * Return the state of the motor safety enabled flag.
   *
   * Return if the motor safety is currently enabled for this device.
   *
   * @return True if motor safety is enforced for this device.
   */
  bool IsSafetyEnabled() const;

  /**
   * Check if this motor has exceeded its timeout.
   *
   * This method is called periodically to determine if this motor has exceeded
   * its timeout value. If it has, the stop method is called, and the motor is
   * shut down until its value is updated again.
   */
  void Check();

  /**
   * Check the motors to see if any have timed out.
   *
   * This static method is called periodically to poll all the motors and stop
   * any that have timed out.
   */
  static void CheckMotors();

  virtual void StopMotor() = 0;
  virtual void GetDescription(wpi::raw_ostream& desc) const = 0;

 private:
  static constexpr double kDefaultSafetyExpiration = 0.1;

  // The expiration time for this object
  double m_expiration = kDefaultSafetyExpiration;

  // True if motor safety is enabled for this motor
  bool m_enabled = false;

  // The FPGA clock value when the motor has expired
  double m_stopTime = Timer::GetFPGATimestamp();

  mutable wpi::mutex m_thisMutex;
};

}  // namespace frc
