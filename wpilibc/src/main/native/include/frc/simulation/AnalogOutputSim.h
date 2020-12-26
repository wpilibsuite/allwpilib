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

  std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  double GetVoltage() const;

  void SetVoltage(double voltage);

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
