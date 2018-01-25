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
 * ADXL345 Accelerometer on SPI.
 *
 * This class allows access to an Analog Devices ADXL345 3-axis accelerometer
 * via SPI. This class assumes the sensor is wired in 4-wire SPI mode.
 */
class ADXL345_SPI : public SensorBase, public Accelerometer {
 public:
  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };

  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

  explicit ADXL345_SPI(SPI::Port port, Range range = kRange_2G);
  ~ADXL345_SPI() override = default;

  ADXL345_SPI(const ADXL345_SPI&) = delete;
  ADXL345_SPI& operator=(const ADXL345_SPI&) = delete;

  // Accelerometer interface
  void SetRange(Range range) override;
  double GetX() override;
  double GetY() override;
  double GetZ() override;

  virtual double GetAcceleration(Axes axis);
  virtual AllAxes GetAccelerations();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  SPI m_spi;

  static constexpr int kPowerCtlRegister = 0x2D;
  static constexpr int kDataFormatRegister = 0x31;
  static constexpr int kDataRegister = 0x32;
  static constexpr double kGsPerLSB = 0.00390625;

  enum SPIAddressFields { kAddress_Read = 0x80, kAddress_MultiByte = 0x40 };

  enum PowerCtlFields {
    kPowerCtl_Link = 0x20,
    kPowerCtl_AutoSleep = 0x10,
    kPowerCtl_Measure = 0x08,
    kPowerCtl_Sleep = 0x04
  };

  enum DataFormatFields {
    kDataFormat_SelfTest = 0x80,
    kDataFormat_SPI = 0x40,
    kDataFormat_IntInvert = 0x20,
    kDataFormat_FullRes = 0x08,
    kDataFormat_Justify = 0x04
  };
};

}  // namespace frc
