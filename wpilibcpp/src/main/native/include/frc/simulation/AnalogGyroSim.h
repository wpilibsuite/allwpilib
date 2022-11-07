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

  /**
   * Register a callback on the angle.
   *
   * @param callback the callback that will be called whenever the angle changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterAngleCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the current angle of the gyro.
   *
   * @return the angle measured by the gyro
   */
  double GetAngle() const;

  /**
   * Change the angle measured by the gyro.
   *
   * @param angle the new value
   */
  void SetAngle(double angle);

  /**
   * Register a callback on the rate.
   *
   * @param callback the callback that will be called whenever the rate changes
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterRateCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the rate of angle change on this gyro.
   *
   * @return the rate
   */
  double GetRate() const;

  /**
   * Change the rate of the gyro.
   *
   * @param rate the new rate
   */
  void SetRate(double rate);

  /**
   * Register a callback on whether the gyro is initialized.
   *
   * @param callback the callback that will be called whenever the gyro is
   *                 initialized
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the gyro is initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Set whether this gyro is initialized.
   *
   * @param initialized the new value
   */
  void SetInitialized(bool initialized);

  /**
   * Reset all simulation data for this object.
   */
  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
