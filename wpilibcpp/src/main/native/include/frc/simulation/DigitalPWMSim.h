// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class DigitalOutput;

namespace sim {

/**
 * Class to control a simulated digital PWM output.
 *
 * This is for duty cycle PWM outputs on a DigitalOutput, not for the servo
 * style PWM outputs on a PWM channel.
 */
class DigitalPWMSim {
 public:
  /**
   * Constructs from a DigitalOutput object.
   *
   * @param digitalOutput DigitalOutput to simulate
   */
  explicit DigitalPWMSim(const DigitalOutput& digitalOutput);

  /**
   * Creates an DigitalPWMSim for a digital I/O channel.
   *
   * @param channel DIO channel
   * @return Simulated object
   * @throws std::out_of_range if no Digital PWM is configured for that channel
   */
  static DigitalPWMSim CreateForChannel(int channel);

  /**
   * Creates an DigitalPWMSim for a simulated index.
   * The index is incremented for each simulated DigitalPWM.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DigitalPWMSim CreateForIndex(int index);

  /**
   * Register a callback to be run when this PWM output is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether this PWM output has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether this PWM output has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run whenever the duty cycle value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterDutyCycleCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the duty cycle value.
   *
   * @return the duty cycle value of this PWM output
   */
  double GetDutyCycle() const;

  /**
   * Set the duty cycle value of this PWM output.
   *
   * @param dutyCycle the new value
   */
  void SetDutyCycle(double dutyCycle);

  /**
   * Register a callback to be run whenever the pin changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPinCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the pin number.
   *
   * @return the pin number
   */
  int GetPin() const;

  /**
   * Change the pin number.
   *
   * @param pin the new pin number
   */
  void SetPin(int pin);

  /**
   * Reset all simulation data.
   */
  void ResetData();

 private:
  explicit DigitalPWMSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
