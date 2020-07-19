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

class DigitalOutput;

namespace sim {

/**
 * Class to control a simulated digital PWM output.
 *
 * This is for duty cycle PWM outputs on a DigitalOutput, not for the servo
 * style PWM outputs on a PWM channel.
 */
class DigitalPWMSim {
 public:
  /**
   * Constructs from a DigitalOutput object.
   *
   * @param digitalOutput DigitalOutput to simulate
   */
  explicit DigitalPWMSim(const DigitalOutput& digitalOutput);

  /**
   * Creates an DigitalPWMSim for a digital I/O channel.
   *
   * @param channel DIO channel
   * @return Simulated object
   * @throws std::out_of_range if no Digital PWM is configured for that channel
   */
  static DigitalPWMSim CreateForChannel(int channel);

  /**
   * Creates an DigitalPWMSim for a simulated index.
   * The index is incremented for each simulated DigitalPWM.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DigitalPWMSim CreateForIndex(int index);

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterDutyCycleCallback(
      NotifyCallback callback, bool initialNotify);

  double GetDutyCycle() const;

  void SetDutyCycle(double dutyCycle);

  std::unique_ptr<CallbackStore> RegisterPinCallback(NotifyCallback callback,
                                                     bool initialNotify);

  int GetPin() const;

  void SetPin(int pin);

  void ResetData();

 private:
  explicit DigitalPWMSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
