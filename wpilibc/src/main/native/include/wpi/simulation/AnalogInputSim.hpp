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
   * Register a callback on the number of average bits.
   *
   * @param callback the callback that will be called whenever the number of
   *                 average bits is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterAverageBitsCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the number of average bits.
   *
   * @return the number of average bits
   */
  int GetAverageBits() const;

  /**
   * Change the number of average bits.
   *
   * @param averageBits the new value
   */
  void SetAverageBits(int averageBits);

  /**
   * Register a callback on the amount of oversampling bits.
   *
   * @param callback the callback that will be called whenever the oversampling
   *                 bits are changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterOversampleBitsCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the amount of oversampling bits.
   *
   * @return the amount of oversampling bits
   */
  int GetOversampleBits() const;

  /**
   * Change the amount of oversampling bits.
   *
   * @param oversampleBits the new value
   */
  void SetOversampleBits(int oversampleBits);

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
