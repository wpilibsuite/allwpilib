// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/length.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/AnalogInput.h"

namespace frc {

class SharpIR : public wpi::Sendable, public wpi::SendableHelper<SharpIR> {
 public:
  /**
   * Sharp GP2Y0A02YK0F is an analog IR sensor capable of measuring
   * distances from 20cm to 150cm.
   *
   * @param channel Analog input channel the sensor is connected to
   *
   * @return sensor object
   */
  static SharpIR GP2Y0A02YK0F(int channel);

  /**
   * Sharp GP2Y0A21YK0F is an analog IR sensor capable of measuring
   * distances from 10cm to 80cm.
   *
   * @param channel Analog input channel the sensor is connected to
   *
   * @return sensor object
   */
  static SharpIR GP2Y0A21YK0F(int channel);

  /**
   * Sharp GP2Y0A41SK0F is an analog IR sensor capable of measuring
   * distances from 4cm to 30cm.
   *
   * @param channel Analog input channel the sensor is connected to
   *
   * @return sensor object
   */
  static SharpIR GP2Y0A41SK0F(int channel);

  /**
   * Sharp GP2Y0A51SK0F is an analog IR sensor capable of measuring
   * distances from 2cm to 15cm.
   *
   * @param channel Analog input channel the sensor is connected to
   *
   * @return sensor object
   */
  static SharpIR GP2Y0A51SK0F(int channel);

  /**
   * Manually construct a SharpIR object. The distance is computed using this
   * formula: A*v ^ B. Prefer to use one of the static factories to create this
   * device instead.
   *
   * @param channel Analog input channel the sensor is connected to
   * @param a Constant A
   * @param b Constant B
   * @param minCM Minimum distance to report in centimeters
   * @param maxCM Maximum distance to report in centimeters
   */
  SharpIR(int channel, double a, double b, double minCM, double maxCM);

  /**
   * Get the analog input channel number.
   *
   * @return analog input channel
   */
  int GetChannel() const;

  /**
   * Get the range from the distance sensor.
   *
   * @return range of the target returned by the sensor
   */
  units::centimeter_t GetRange() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  AnalogInput m_sensor;

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simRange;

  double m_A;
  double m_B;
  double m_minCM;
  double m_maxCM;
};

}  // namespace frc
