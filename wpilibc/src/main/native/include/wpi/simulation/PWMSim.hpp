// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/simulation/CallbackStore.hpp"

namespace wpi {

class PWMOutput;
class PWMMotorController;

namespace sim {

/**
 * Class to control a simulated PWMOutput output.
 */
class PWMSim {
 public:
  /**
   * Constructs from a PWMOutput object.
   *
   * @param pwm PWMOutput to simulate
   */
  explicit PWMSim(const PWMOutput& pwm);

  /**
   * Constructs from a PWMOutput channel number.
   *
   * @param channel Channel number
   */
  explicit PWMSim(int channel);

  /**
   * Register a callback to be run when the PWMOutput is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the PWMOutput has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether the PWMOutput has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run when the PWMOutput pulse microsecond value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterPulseMicrosecondCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWMOutput pulse microsecond value.
   *
   * @return the PWMOutput pulse microsecond value
   */
  int32_t GetPulseMicrosecond() const;

  /**
   * Set the PWMOutput pulse microsecond value.
   *
   * @param microsecondPulseTime the PWMOutput pulse microsecond value
   */
  void SetPulseMicrosecond(int32_t microsecondPulseTime);

  /**
   * Register a callback to be run when the PWMOutput period scale changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterOutputPeriodCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the PWMOutput period scale.
   *
   * @return the PWMOutput period scale
   */
  int GetOutputPeriod() const;

  /**
   * Set the PWMOutput period scale.
   *
   * @param period the PWMOutput period scale
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
}  // namespace wpi
