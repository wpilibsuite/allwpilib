/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/AnalogAccelerometer.h"
#include "frc/AnalogInput.h"
#include "frc/ErrorBase.h"
#include "frc/experimental/controller/MeasurementSource.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {
namespace experimental {

/**
 * Handle operation of an analog accelerometer.
 *
 * The accelerometer reads acceleration directly through the sensor. Many
 * sensors have multiple axis and can be treated as multiple devices. Each is
 * calibrated by finding the center value over a period of time.
 */
class AnalogAccelerometer : public ErrorBase,
                            public SendableBase,
                            public MeasurementSource {
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
  explicit AnalogAccelerometer(AnalogInput& channel);

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
   * acceleration. The sensitivity varies by accelerometer model. There are
   * constants defined for various models.
   *
   * @param sensitivity The sensitivity of accelerometer in Volts per G.
   */
  void SetSensitivity(double sensitivity);

  /**
   * Sets the voltage that corresponds to 0 G.
   *
   * The zero G voltage varies by accelerometer model. There are constants
   * defined for various models.
   *
   * @param zero The zero G voltage.
   */
  void SetZero(double zero);

  /**
   * Gets the acceleration.
   *
   * @return The current acceleration in Gs.
   */
  double GetMeasurement() const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  frc::AnalogAccelerometer m_impl;
};

}  // namespace experimental
}  // namespace frc
