// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class DutyCycle;

namespace sim {

/**
 * Class to control a simulated duty cycle digital input.
 */
class DutyCycleSim {
 public:
  /**
   * Constructs from a DutyCycle object.
   *
   * @param dutyCycle DutyCycle to simulate
   */
  explicit DutyCycleSim(const DutyCycle& dutyCycle);

  /**
   * Creates a DutyCycleSim for a digital input channel.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws std::out_of_range if no DutyCycle is configured for that channel
   */
  static DutyCycleSim CreateForChannel(int channel);

  /**
   * Creates a DutyCycleSim for a simulated index.
   * The index is incremented for each simulated DutyCycle.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DutyCycleSim CreateForIndex(int index);

  /**
   * Register a callback to be run when this duty cycle input is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether this duty cycle input has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether this duty cycle input has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run whenever the frequency changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterFrequencyCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the frequency.
   *
   * @return the duty cycle frequency
   */
  int GetFrequency() const;

  /**
   * Change the duty cycle frequency.
   *
   * @param frequency the new frequency
   */
  void SetFrequency(int count);

  /**
   * Register a callback to be run whenever the output changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterOutputCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the output from this duty cycle port.
   *
   * @return the output value
   */
  double GetOutput() const;

  /**
   * Change the duty cycle output.
   *
   * @param output the new output value
   */
  void SetOutput(double period);

  /**
   * Reset all simulation data for the duty cycle output.
   */
  void ResetData();

 private:
  explicit DutyCycleSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
