// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class AnalogInput;

namespace sim {

/**
 * Class to control a simulated analog input.
 */
class AnalogInputSim {
 public:
  /**
   * Constructs from an AnalogInput object.
   *
   * @param analogInput AnalogInput to simulate
   */
  explicit AnalogInputSim(const AnalogInput& analogInput);

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  explicit AnalogInputSim(int channel);

  /**
   * Register a callback on whether the analog input is initialized.
   *
   * @param callback the callback that will be called whenever the analog input
   *                 is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if this analog input has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Change whether this analog input has been initialized.
   *
   * @param initialized the new value
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback on the voltage.
   *
   * @param callback the callback that will be called whenever the voltage is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the voltage.
   *
   * @return the voltage
   */
  double GetVoltage() const;

  /**
   * Change the voltage.
   *
   * @param voltage the new value
   */
  void SetVoltage(double voltage);

  /**
   * Reset all simulation data for this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
