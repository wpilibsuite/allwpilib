/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "I2C.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "interfaces/Accelerometer.h"

namespace frc {

/**
 * ADXL345 Accelerometer on I2C.
 *
 * This class allows access to a Analog Devices ADXL345 3-axis accelerometer on
 * an I2C bus.
 * This class assumes the default (not alternate) sensor address of 0x1D (7-bit
 * address).
 */
class ADXL345_I2C : public Accelerometer, public LiveWindowSendable {
 protected:
  static const int kAddress = 0x1D;
  static const int kPowerCtlRegister = 0x2D;
  static const int kDataFormatRegister = 0x31;
  static const int kDataRegister = 0x32;
  static constexpr double kGsPerLSB = 0.00390625;
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

 public:
  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };
  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

 public:
  explicit ADXL345_I2C(I2C::Port port, Range range = kRange_2G,
                       int deviceAddress = kAddress);
  virtual ~ADXL345_I2C() = default;

  ADXL345_I2C(const ADXL345_I2C&) = delete;
  ADXL345_I2C& operator=(const ADXL345_I2C&) = delete;

  // Accelerometer interface
  void SetRange(Range range) override;
  double GetX() override;
  double GetY() override;
  double GetZ() override;

  virtual double GetAcceleration(Axes axis);
  virtual AllAxes GetAccelerations();

  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subtable) override;
  void UpdateTable() override;
  std::shared_ptr<ITable> GetTable() const override;
  void StartLiveWindowMode() override {}
  void StopLiveWindowMode() override {}

 protected:
  I2C m_i2c;

 private:
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
