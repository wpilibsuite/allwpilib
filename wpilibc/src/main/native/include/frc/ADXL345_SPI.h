// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>

#include "frc/ErrorBase.h"
#include "frc/SPI.h"
#include "frc/interfaces/Accelerometer.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

/**
 * ADXL345 Accelerometer on SPI.
 *
 * This class allows access to an Analog Devices ADXL345 3-axis accelerometer
 * via SPI. This class assumes the sensor is wired in 4-wire SPI mode.
 */
class ADXL345_SPI : public ErrorBase,
                    public Accelerometer,
                    public Sendable,
                    public SendableHelper<ADXL345_SPI> {
 public:
  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };

  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

  /**
   * Constructor.
   *
   * @param port  The SPI port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure
   */
  explicit ADXL345_SPI(SPI::Port port, Range range = kRange_2G);

  ~ADXL345_SPI() override = default;

  ADXL345_SPI(ADXL345_SPI&&) = default;
  ADXL345_SPI& operator=(ADXL345_SPI&&) = default;

  // Accelerometer interface
  void SetRange(Range range) final;
  double GetX() override;
  double GetY() override;
  double GetZ() override;

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  virtual double GetAcceleration(Axes axis);

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  virtual AllAxes GetAccelerations();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  SPI m_spi;

  hal::SimDevice m_simDevice;
  hal::SimEnum m_simRange;
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;

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
