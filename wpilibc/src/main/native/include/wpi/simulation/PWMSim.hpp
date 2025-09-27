// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/simulation/CallbackStore.hpp"

namespace frc {

class PWM;
class PWMMotorController;

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
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
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
   * Register a callback to be run when the PWM pulse microsecond value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterPulseMicrosecondCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM pulse microsecond value.
   *
   * @return the PWM pulse microsecond value
   */
  int32_t GetPulseMicrosecond() const;

  /**
   * Set the PWM pulse microsecond value.
   *
   * @param microsecondPulseTime the PWM pulse microsecond value
   */
  void SetPulseMicrosecond(int32_t microsecondPulseTime);

  /**
   * Register a callback to be run when the PWM period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterOutputPeriodCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWM period scale.
   *
   * @return the PWM period scale
   */
  int GetOutputPeriod() const;

  /**
   * Set the PWM period scale.
   *
   * @param period the PWM period scale
   */
  void SetOutputPeriod(int period);

  /**
   * Reset all simulation data.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
