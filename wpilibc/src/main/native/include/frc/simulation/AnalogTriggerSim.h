// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class AnalogTrigger;

namespace sim {

/**
 * Class to control a simulated analog trigger.
 */
class AnalogTriggerSim {
 public:
  /**
   * Constructs from an AnalogTrigger object.
   *
   * @param analogTrigger AnalogTrigger to simulate
   */
  explicit AnalogTriggerSim(const AnalogTrigger& analogTrigger);

  /**
   * Creates an AnalogTriggerSim for an analog input channel.
   *
   * @param channel analog input channel
   * @return Simulated object
   * @throws std::out_of_range if no AnalogTrigger is configured for that
   *         channel
   */
  static AnalogTriggerSim CreateForChannel(int channel);

  /**
   * Creates an AnalogTriggerSim for a simulated index.
   * The index is incremented for each simulated AnalogTrigger.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static AnalogTriggerSim CreateForIndex(int index);

  /**
   * Register a callback on whether the analog trigger is initialized.
   *
   * @param callback the callback that will be called whenever the analog
   *                 trigger is initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if this analog trigger has been initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Change whether this analog trigger has been initialized.
   *
   * @param initialized the new value
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback on the lower bound.
   *
   * @param callback the callback that will be called whenever the lower bound
   *                 is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterTriggerLowerBoundCallback(NotifyCallback callback,
                                    bool initialNotify);

  /**
   * Get the lower bound.
   *
   * @return the lower bound
   */
  double GetTriggerLowerBound() const;

  /**
   * Change the lower bound.
   *
   * @param triggerLowerBound the new lower bound
   */
  void SetTriggerLowerBound(double triggerLowerBound);

  /**
   * Register a callback on the upper bound.
   *
   * @param callback the callback that will be called whenever the upper bound
   *                 is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore>
  RegisterTriggerUpperBoundCallback(NotifyCallback callback,
                                    bool initialNotify);

  /**
   * Get the upper bound.
   *
   * @return the upper bound
   */
  double GetTriggerUpperBound() const;

  /**
   * Change the upper bound.
   *
   * @param triggerUpperBound the new upper bound
   */
  void SetTriggerUpperBound(double triggerUpperBound);

  /**
   * Reset all simulation data for this object.
   */
  void ResetData();

 private:
  explicit AnalogTriggerSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
