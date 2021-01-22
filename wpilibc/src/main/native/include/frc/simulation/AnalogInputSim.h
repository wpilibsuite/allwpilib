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
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
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
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterAverageBitsCallback(
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
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterOversampleBitsCallback(
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
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterVoltageCallback(
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
   * Register a callback on whether the accumulator is initialized.
   *
   * @param callback the callback that will be called whenever the accumulator
   *                 is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterAccumulatorInitializedCallback(NotifyCallback callback,
                                         bool initialNotify);

  /**
   * Check if the accumulator has been initialized.
   *
   * @return true if initialized
   */
  bool GetAccumulatorInitialized() const;

  /**
   * Change whether the accumulator has been initialized.
   *
   * @param accumulatorInitialized the new value
   */
  void SetAccumulatorInitialized(bool accumulatorInitialized);

  /**
   * Register a callback on the accumulator value.
   *
   * @param callback the callback that will be called whenever the accumulator
   *                 value is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterAccumulatorValueCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the accumulator value.
   *
   * @return the accumulator value
   */
  int64_t GetAccumulatorValue() const;

  /**
   * Change the accumulator value.
   *
   * @param accumulatorValue the new value
   */
  void SetAccumulatorValue(int64_t accumulatorValue);

  /**
   * Register a callback on the accumulator count.
   *
   * @param callback the callback that will be called whenever the accumulator
   *                 count is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterAccumulatorCountCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the accumulator count.
   *
   * @return the accumulator count.
   */
  int64_t GetAccumulatorCount() const;

  /**
   * Change the accumulator count.
   *
   * @param accumulatorCount the new count.
   */
  void SetAccumulatorCount(int64_t accumulatorCount);

  /**
   * Register a callback on the accumulator center.
   *
   * @param callback the callback that will be called whenever the accumulator
   *                 center is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterAccumulatorCenterCallback(NotifyCallback callback,
                                    bool initialNotify);

  /**
   * Get the accumulator center.
   *
   * @return the accumulator center
   */
  int GetAccumulatorCenter() const;

  /**
   * Change the accumulator center.
   *
   * @param accumulatorCenter the new center
   */
  void SetAccumulatorCenter(int accumulatorCenter);

  /**
   * Register a callback on the accumulator deadband.
   *
   * @param callback the callback that will be called whenever the accumulator
   *                 deadband is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterAccumulatorDeadbandCallback(NotifyCallback callback,
                                      bool initialNotify);

  /**
   * Get the accumulator deadband.
   *
   * @return the accumulator deadband
   */
  int GetAccumulatorDeadband() const;

  /**
   * Change the accumulator deadband.
   *
   * @param accumulatorDeadband the new deadband
   */
  void SetAccumulatorDeadband(int accumulatorDeadband);

  /**
   * Reset all simulation data for this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
