/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "interfaces/Accelerometer.h"
#include "I2C.h"
#include "LiveWindow/LiveWindowSendable.h"
#include <memory>

/**
 * ADXL345 Accelerometer on I2C.
 *
 * This class allows access to a Analog Devices ADXL345 3-axis accelerometer on
 * an I2C bus.
 * This class assumes the default (not alternate) sensor address of 0x1D (7-bit
 * address).
 */
class ADXL345_I2C : public Accelerometer,
                    public I2C,
                    public LiveWindowSendable {
 protected:
  static const uint8_t kAddress = 0x1D;
  static const uint8_t kPowerCtlRegister = 0x2D;
  static const uint8_t kDataFormatRegister = 0x31;
  static const uint8_t kDataRegister = 0x32;
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
  explicit ADXL345_I2C(Port port, Range range = kRange_2G);
  virtual ~ADXL345_I2C() = default;

  ADXL345_I2C(const ADXL345_I2C&) = delete;
  ADXL345_I2C& operator=(const ADXL345_I2C&) = delete;

  // Accelerometer interface
  virtual void SetRange(Range range) override;
  virtual double GetX() override;
  virtual double GetY() override;
  virtual double GetZ() override;

  virtual double GetAcceleration(Axes axis);
  virtual AllAxes GetAccelerations();

  virtual std::string GetSmartDashboardType() const override;
  virtual void InitTable(std::shared_ptr<ITable> subtable) override;
  virtual void UpdateTable() override;
  virtual std::shared_ptr<ITable> GetTable() const override;
  virtual void StartLiveWindowMode() override {}
  virtual void StopLiveWindowMode() override {}

 private:
  std::shared_ptr<ITable> m_table;
};
