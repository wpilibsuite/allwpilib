/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterTriggerLowerBoundCallback(
      NotifyCallback callback, bool initialNotify);

  double GetTriggerLowerBound() const;

  void SetTriggerLowerBound(double triggerLowerBound);

  std::unique_ptr<CallbackStore> RegisterTriggerUpperBoundCallback(
      NotifyCallback callback, bool initialNotify);

  double GetTriggerUpperBound() const;

  void SetTriggerUpperBound(double triggerUpperBound);

  void ResetData();

 private:
  explicit AnalogTriggerSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
