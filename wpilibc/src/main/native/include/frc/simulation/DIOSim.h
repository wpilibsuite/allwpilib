// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class DigitalInput;
class DigitalOutput;

namespace sim {

/**
 * Class to control a simulated digital input or output.
 */
class DIOSim {
 public:
  /**
   * Constructs from a DigitalInput object.
   *
   * @param input DigitalInput to simulate
   */
  explicit DIOSim(const DigitalInput& input);

  /**
   * Constructs from a DigitalOutput object.
   *
   * @param output DigitalOutput to simulate
   */
  explicit DIOSim(const DigitalOutput& output);

  /**
   * Constructs from an digital I/O channel number.
   *
   * @param channel Channel number
   */
  explicit DIOSim(int channel);

  /**
   * Register a callback to be run when this DIO is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether this DIO has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether this DIO has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run whenever the DIO value changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterValueCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the value of the DIO port.
   *
   * @return the DIO value
   */
  bool GetValue() const;

  /**
   * Change the DIO value.
   *
   * @param value the new value
   */
  void SetValue(bool value);

  /**
   * Register a callback to be run whenever the pulse length changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPulseLengthCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the pulse length.
   *
   * @return the pulse length of this DIO port
   */
  double GetPulseLength() const;

  /**
   * Change the pulse length of this DIO port.
   *
   * @param pulseLength the new pulse length
   */
  void SetPulseLength(double pulseLength);

  /**
   * Register a callback to be run whenever this DIO changes to be an input.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterIsInputCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether this DIO port is currently an Input.
   *
   * @return true if Input
   */
  bool GetIsInput() const;

  /**
   * Define whether this DIO port is an Input.
   *
   * @param isInput whether this DIO should be an Input
   */
  void SetIsInput(bool isInput);

  /**
   * Register a callback to be run whenever the filter index changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterFilterIndexCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the filter index.
   *
   * @return the filter index of this DIO port
   */
  int GetFilterIndex() const;

  /**
   * Change the filter index of this DIO port.
   *
   * @param filterIndex the new filter index
   */
  void SetFilterIndex(int filterIndex);

  /**
   * Reset all simulation data of this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
