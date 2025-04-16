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
   * @param min Minimum distance to report
   * @param max Maximum distance to report
   */
  SharpIR(int channel, double a, double b, units::meter_t min,
          units::meter_t max);

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
  units::meter_t GetRange() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  AnalogInput m_sensor;

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simRange;

  double m_A;
  double m_B;
  units::meter_t m_min;
  units::meter_t m_max;
};

}  // namespace frc
