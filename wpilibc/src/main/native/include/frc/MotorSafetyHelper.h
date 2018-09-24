/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/mutex.h>

#include "frc/ErrorBase.h"

namespace frc {

class MotorSafety;

class MotorSafetyHelper : public ErrorBase {
 public:
  /**
   * The constructor for a MotorSafetyHelper object.
   *
   * The helper object is constructed for every object that wants to implement
   * the Motor Safety protocol. The helper object has the code to actually do
   * the timing and call the motors Stop() method when the timeout expires. The
   * motor object is expected to call the Feed() method whenever the motors
   * value is updated.
   *
   * @param safeObject a pointer to the motor object implementing MotorSafety.
   *                   This is used to call the Stop() method on the motor.
   */
  explicit MotorSafetyHelper(MotorSafety* safeObject);

  ~MotorSafetyHelper();

  MotorSafetyHelper(MotorSafetyHelper&&) = default;
  MotorSafetyHelper& operator=(MotorSafetyHelper&&) = default;

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
   * @return a true value if the motor is still operating normally and hasn't
   * timed out.
   */
  bool IsAlive() const;

  /**
   * Check if this motor has exceeded its timeout.
   *
   * This method is called periodically to determine if this motor has exceeded
   * its timeout value. If it has, the stop method is called, and the motor is
   * shut down until its value is updated again.
   */
  void Check();

  /**
   * Enable/disable motor safety for this device
   *
   * Turn on and off the motor safety option for this PWM object.
   *
   * @param enabled True if motor safety is enforced for this object
   */
  void SetSafetyEnabled(bool enabled);

  /**
   * Return the state of the motor safety enabled flag
   *
   * Return if the motor safety is currently enabled for this devicce.
   *
   * @return True if motor safety is enforced for this device
   */
  bool IsSafetyEnabled() const;

  /**
   * Check the motors to see if any have timed out.
   *
   * This static method is called periodically to poll all the motors and stop
   * any that have timed out.
   */
  static void CheckMotors();

 private:
  // The expiration time for this object
  double m_expiration;

  // True if motor safety is enabled for this motor
  bool m_enabled;

  // The FPGA clock value when this motor has expired
  double m_stopTime;

  // Protect accesses to the state for this object
  mutable wpi::mutex m_thisMutex;

  // The object that is using the helper
  MotorSafety* m_safeObject;
};

}  // namespace frc
