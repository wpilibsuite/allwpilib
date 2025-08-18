// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/AnalogInput.h"

namespace frc {

/**
 * Handle operation of an analog accelerometer.
 *
 * The accelerometer reads acceleration directly through the sensor. Many
 * sensors have multiple axis and can be treated as multiple devices. Each is
 * calibrated by finding the center value over a period of time.
 */
class AnalogAccelerometer : public wpi::Sendable,
                            public wpi::SendableHelper<AnalogAccelerometer> {
 public:
  /**
   * Create a new instance of an accelerometer.
   *
   * The constructor allocates desired analog input.
   *
   * @param channel The channel number for the analog input the accelerometer is
   *                connected to
   */
  explicit AnalogAccelerometer(int channel);

  /**
   * Create a new instance of Accelerometer from an existing AnalogInput.
   *
   * Make a new instance of accelerometer given an AnalogInput. This is
   * particularly useful if the port is going to be read as an analog channel as
   * well as through the Accelerometer class.
   *
   * @param channel The existing AnalogInput object for the analog input the
   *                accelerometer is connected to
   */
  explicit AnalogAccelerometer(AnalogInput* channel);

  /**
   * Create a new instance of Accelerometer from an existing AnalogInput.
   *
   * Make a new instance of accelerometer given an AnalogInput. This is
   * particularly useful if the port is going to be read as an analog channel as
   * well as through the Accelerometer class.
   *
   * @param channel The existing AnalogInput object for the analog input the
   *                accelerometer is connected to
   */
  explicit AnalogAccelerometer(std::shared_ptr<AnalogInput> channel);

  ~AnalogAccelerometer() override = default;

  AnalogAccelerometer(AnalogAccelerometer&&) = default;
  AnalogAccelerometer& operator=(AnalogAccelerometer&&) = default;

  /**
   * Return the acceleration in Gs.
   *
   * The acceleration is returned units of Gs.
   *
   * @return The current acceleration of the sensor in Gs.
   */
  double GetAcceleration() const;

  /**
   * Set the accelerometer sensitivity.
   *
   * This sets the sensitivity of the accelerometer used for calculating the
   * acceleration. The sensitivity varies by accelerometer model.
   *
   * @param sensitivity The sensitivity of accelerometer in Volts per G.
   */
  void SetSensitivity(double sensitivity);

  /**
   * Set the voltage that corresponds to 0 G.
   *
   * The zero G voltage varies by accelerometer model.
   *
   * @param zero The zero G voltage.
   */
  void SetZero(double zero);

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  /**
   * Common function for initializing the accelerometer.
   */
  void InitAccelerometer();

  std::shared_ptr<AnalogInput> m_analogInput;
  double m_voltsPerG = 1.0;
  double m_zeroGVoltage = 2.5;
};

}  // namespace frc
