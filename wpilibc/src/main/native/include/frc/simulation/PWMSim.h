// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class PWM;

namespace sim {

/**
 * Class to control a simulated PWM output.
 */
class PWMSim {
 public:
  /**
   * Constructs from a PWM object.
   *
   * @param pwm PWM to simulate
   */
  explicit PWMSim(const PWM& pwm);

  /**
   * Constructs from a PWM channel number.
   *
   * @param channel Channel number
   */
  explicit PWMSim(int channel);

  /**
   * Register a callback to be run when the PWM is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the PWM has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether the PWM has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run when the PWM raw value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterRawValueCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM raw value.
   *
   * @return the PWM raw value
   */
  int GetRawValue() const;

  /**
   * Set the PWM raw value.
   *
   * @param rawValue the PWM raw value
   */
  void SetRawValue(int rawValue);

  /**
   * Register a callback to be run when the PWM speed changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterSpeedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM speed.
   *
   * @return the PWM speed (-1.0 to 1.0)
   */
  double GetSpeed() const;

  /**
   * Set the PWM speed.
   *
   * @param speed the PWM speed (-1.0 to 1.0)
   */
  void SetSpeed(double speed);

  /**
   * Register a callback to be run when the PWM position changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPositionCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM position.
   *
   * @return the PWM position (0.0 to 1.0)
   */
  double GetPosition() const;

  /**
   * Set the PWM position.
   *
   * @param position the PWM position (0.0 to 1.0)
   */
  void SetPosition(double position);

  /**
   * Register a callback to be run when the PWM period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPeriodScaleCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM period scale.
   *
   * @return the PWM period scale
   */
  int GetPeriodScale() const;

  /**
   * Set the PWM period scale.
   *
   * @param periodScale the PWM period scale
   */
  void SetPeriodScale(int periodScale);

  /**
   * Register a callback to be run when the PWM zero latch state changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterZeroLatchCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the PWM is zero latched.
   *
   * @return true if zero latched
   */
  bool GetZeroLatch() const;

  /**
   * Define whether the PWM has been zero latched.
   *
   * @param zeroLatch true to indicate zero latched
   */
  void SetZeroLatch(bool zeroLatch);

  /**
   * Reset all simulation data.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
