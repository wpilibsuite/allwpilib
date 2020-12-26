// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class PWM;

namespace sim {

/**
 * Class to control a simulated PWM output.
 */
class PWMSim {
 public:
  /**
   * Constructs from a PWM object.
   *
   * @param pwm PWM to simulate
   */
  explicit PWMSim(const PWM& pwm);

  /**
   * Constructs from a PWM channel number.
   *
   * @param channel Channel number
   */
  explicit PWMSim(int channel);

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterRawValueCallback(
      NotifyCallback callback, bool initialNotify);

  int GetRawValue() const;

  void SetRawValue(int rawValue);

  std::unique_ptr<CallbackStore> RegisterSpeedCallback(NotifyCallback callback,
                                                       bool initialNotify);

  double GetSpeed() const;

  void SetSpeed(double speed);

  std::unique_ptr<CallbackStore> RegisterPositionCallback(
      NotifyCallback callback, bool initialNotify);

  double GetPosition() const;

  void SetPosition(double position);

  std::unique_ptr<CallbackStore> RegisterPeriodScaleCallback(
      NotifyCallback callback, bool initialNotify);

  int GetPeriodScale() const;

  void SetPeriodScale(int periodScale);

  std::unique_ptr<CallbackStore> RegisterZeroLatchCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetZeroLatch() const;

  void SetZeroLatch(bool zeroLatch);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
