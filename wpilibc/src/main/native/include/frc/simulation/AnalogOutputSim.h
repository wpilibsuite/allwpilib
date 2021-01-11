// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class AnalogOutput;

namespace sim {

/**
 * Class to control a simulated analog output.
 */
class AnalogOutputSim {
 public:
  /**
   * Constructs from an AnalogOutput object.
   *
   * @param analogOutput AnalogOutput to simulate
   */
  explicit AnalogOutputSim(const AnalogOutput& analogOutput);

  /**
   * Constructs from an analog output channel number.
   *
   * @param channel Channel number
   */
  explicit AnalogOutputSim(int channel);

  /**
   * Register a callback to be run whenever the voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the analog output voltage.
   *
   * @return the voltage on this analog output
   */
  double GetVoltage() const;

  /**
   * Set the analog output voltage.
   *
   * @param voltage the new voltage on this analog output
   */
  void SetVoltage(double voltage);

  /**
   * Register a callback to be run when this analog output is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether this analog output has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether this analog output has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Reset all simulation data on this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
