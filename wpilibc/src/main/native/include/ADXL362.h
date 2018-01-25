/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SPI.h"
#include "SensorBase.h"
#include "interfaces/Accelerometer.h"

namespace frc {

/**
 * ADXL362 SPI Accelerometer.
 *
 * This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
class ADXL362 : public SensorBase, public Accelerometer {
 public:
  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };
  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

 public:
  explicit ADXL362(Range range = kRange_2G);
  explicit ADXL362(SPI::Port port, Range range = kRange_2G);
  virtual ~ADXL362() = default;

  ADXL362(const ADXL362&) = delete;
  ADXL362& operator=(const ADXL362&) = delete;

  // Accelerometer interface
  void SetRange(Range range) override;
  double GetX() override;
  double GetY() override;
  double GetZ() override;

  virtual double GetAcceleration(Axes axis);
  virtual AllAxes GetAccelerations();

  void InitSendable(SendableBuilder& builder) override;

 private:
  SPI m_spi;
  double m_gsPerLSB = 0.001;
};

}  // namespace frc
