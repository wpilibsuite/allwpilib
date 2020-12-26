// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class AnalogGyro;

namespace sim {

/**
 * Class to control a simulated analog gyro.
 */
class AnalogGyroSim {
 public:
  /**
   * Constructs from an AnalogGyro object.
   *
   * @param gyro AnalogGyro to simulate
   */
  explicit AnalogGyroSim(const AnalogGyro& gyro);

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  explicit AnalogGyroSim(int channel);

  std::unique_ptr<CallbackStore> RegisterAngleCallback(NotifyCallback callback,
                                                       bool initialNotify);

  double GetAngle() const;

  void SetAngle(double angle);

  std::unique_ptr<CallbackStore> RegisterRateCallback(NotifyCallback callback,
                                                      bool initialNotify);

  double GetRate() const;

  void SetRate(double rate);

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
