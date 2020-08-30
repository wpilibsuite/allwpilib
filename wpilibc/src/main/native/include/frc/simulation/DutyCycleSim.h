/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class DutyCycle;

namespace sim {

/**
 * Class to control a simulated duty cycle digital input.
 */
class DutyCycleSim {
 public:
  /**
   * Constructs from a DutyCycle object.
   *
   * @param dutyCycle DutyCycle to simulate
   */
  explicit DutyCycleSim(const DutyCycle& dutyCycle);

  /**
   * Creates a DutyCycleSim for a digital input channel.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws std::out_of_range if no DutyCycle is configured for that channel
   */
  static DutyCycleSim CreateForChannel(int channel);

  /**
   * Creates a DutyCycleSim for a simulated index.
   * The index is incremented for each simulated DutyCycle.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DutyCycleSim CreateForIndex(int index);

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterFrequencyCallback(
      NotifyCallback callback, bool initialNotify);

  int GetFrequency() const;

  void SetFrequency(int count);

  std::unique_ptr<CallbackStore> RegisterOutputCallback(NotifyCallback callback,
                                                        bool initialNotify);

  double GetOutput() const;

  void SetOutput(double period);

  void ResetData();

 private:
  explicit DutyCycleSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
