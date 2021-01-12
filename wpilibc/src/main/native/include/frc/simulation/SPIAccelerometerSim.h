// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc::sim {
class SPIAccelerometerSim {
 public:
  /**
   * Construct a new simulation object.
   *
   * @param index the HAL index of the accelerometer
   */
  explicit SPIAccelerometerSim(int index);

  /**
   * Register a callback to be run when this accelerometer activates.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterActiveCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check whether the accelerometer is active.
   *
   * @return true if active
   */
  bool GetActive() const;

  /**
   * Define whether this accelerometer is active.
   *
   * @param active the new state
   */
  void SetActive(bool active);

  /**
   * Register a callback to be run whenever the range changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterRangeCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check the range of this accelerometer.
   *
   * @return the accelerometer range
   */
  int GetRange() const;

  /**
   * Change the range of this accelerometer.
   *
   * @param range the new accelerometer range
   */
  void SetRange(int range);

  /**
   * Register a callback to be run whenever the X axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterXCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the X axis value.
   *
   * @return the X axis measurement
   */
  double GetX() const;

  /**
   * Change the X axis value of the accelerometer.
   *
   * @param x the new reading of the X axis
   */
  void SetX(double x);

  /**
   * Register a callback to be run whenever the Y axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterYCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the Y axis value.
   *
   * @return the Y axis measurement
   */
  double GetY() const;

  /**
   * Change the Y axis value of the accelerometer.
   *
   * @param y the new reading of the Y axis
   */
  void SetY(double y);

  /**
   * Register a callback to be run whenever the Z axis value changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterZCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the Z axis value.
   *
   * @return the Z axis measurement
   */
  double GetZ() const;

  /**
   * Change the Z axis value of the accelerometer.
   *
   * @param z the new reading of the Z axis
   */
  void SetZ(double z);

  /**
   * Reset all simulation data of this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace frc::sim
