// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class PowerDistribution;

namespace sim {

/**
 * Class to control a simulated Power Distribution Panel (PowerDistribution).
 */
class PowerDistributionSim {
 public:
  /**
   * Constructs from a PowerDistribution module number (CAN ID).
   *
   * @param module module number
   */
  explicit PowerDistributionSim(int module = 0);

  /**
   * Constructs from a PowerDistribution object.
   *
   * @param pdp PowerDistribution to simulate
   */
  explicit PowerDistributionSim(const PowerDistribution& pdp);

  /**
   * Register a callback to be run when the PowerDistribution is initialized.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the PowerDistribution has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Define whether the PowerDistribution has been initialized.
   *
   * @param initialized whether this object is initialized
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback to be run whenever the PowerDistribution temperature
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterTemperatureCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the temperature of the PowerDistribution.
   *
   * @return the PowerDistribution temperature
   */
  double GetTemperature() const;

  /**
   * Define the PowerDistribution temperature.
   *
   * @param temperature the new PowerDistribution temperature
   */
  void SetTemperature(double temperature);

  /**
   * Register a callback to be run whenever the PowerDistribution voltage
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the PowerDistribution voltage.
   *
   * @return the PowerDistribution voltage.
   */
  double GetVoltage() const;

  /**
   * Set the PowerDistribution voltage.
   *
   * @param voltage the new PowerDistribution voltage
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
   * Read the current in one of the PowerDistribution channels.
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
   * Read the current of all of the PowerDistribution channels.
   *
   * @param currents output array; set to the current in each channel. The
   *                 array must be big enough to hold all the PowerDistribution
   *                 channels
   * @param length   length of output array
   */
  void GetAllCurrents(double* currents, int length) const;

  /**
   * Change the current in all of the PowerDistribution channels.
   *
   * @param currents array containing the current values for each channel. The
   *                 array must be big enough to hold all the PowerDistribution
   *                 channels
   * @param length   length of array
   */
  void SetAllCurrents(const double* currents, int length);

  /**
   * Reset all PowerDistribution simulation data.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
