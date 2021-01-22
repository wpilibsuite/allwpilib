// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class PowerDistributionPanel;

namespace sim {

/**
 * Class to control a simulated Power Distribution Panel (PDP).
 */
class PDPSim {
 public:
  /**
   * Constructs from a PDP module number (CAN ID).
   *
   * @param module module number
   */
  explicit PDPSim(int module = 0);

  /**
   * Constructs from a PowerDistributionPanel object.
   *
   * @param pdp PowerDistributionPanel to simulate
   */
  explicit PDPSim(const PowerDistributionPanel& pdp);

  /**
   * Register a callback to be run when the PDP is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the PDP has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether the PDP has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run whenever the PDP temperature changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterTemperatureCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the temperature of the PDP.
   *
   * @return the PDP temperature
   */
  double GetTemperature() const;

  /**
   * Define the PDP temperature.
   *
   * @param temperature the new PDP temperature
   */
  void SetTemperature(double temperature);

  /**
   * Register a callback to be run whenever the PDP voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the PDP voltage.
   *
   * @return the PDP voltage.
   */
  double GetVoltage() const;

  /**
   * Set the PDP voltage.
   *
   * @param voltage the new PDP voltage
   */
  void SetVoltage(double voltage);

  /**
   * Register a callback to be run whenever the current of a specific channel
   * changes.
   *
   * @param channel the channel
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterCurrentCallback(
      int channel, NotifyCallback callback, bool initialNotify);

  /**
   * Read the current in one of the PDP channels.
   *
   * @param channel the channel to check
   * @return the current in the given channel
   */
  double GetCurrent(int channel) const;

  /**
   * Change the current in the given channel.
   *
   * @param channel the channel to edit
   * @param current the new current for the channel
   */
  void SetCurrent(int channel, double current);

  /**
   * Read the current of all of the PDP channels.
   *
   * @param currents output array; set to the current in each channel. The
   *                 array must be big enough to hold all the PDP channels
   */
  void GetAllCurrents(double* currents) const;

  /**
   * Change the current in all of the PDP channels.
   *
   * @param currents array containing the current values for each channel. The
   *                 array must be big enough to hold all the PDP channels
   */
  void SetAllCurrents(const double* currents);

  /**
   * Reset all PDP simulation data.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
