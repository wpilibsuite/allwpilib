/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "interfaces/Accelerometer.h"
#include "SensorBase.h"
#include "SPI.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

class DigitalInput;
class DigitalOutput;

/**
 * ADXL362 SPI Accelerometer.
 *
 * This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
class ADXL362 : public Accelerometer, public LiveWindowSendable {
 public:
  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };
  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

 public:
  ADXL362(Range range = kRange_2G);
  ADXL362(SPI::Port port, Range range = kRange_2G);
  virtual ~ADXL362() = default;

  ADXL362(const ADXL362&) = delete;
  ADXL362& operator=(const ADXL362&) = delete;

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
  SPI m_spi;
  double m_gsPerLSB = 0.001;

  std::shared_ptr<ITable> m_table;
};
