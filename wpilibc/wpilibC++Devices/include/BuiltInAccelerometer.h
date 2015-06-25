/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "interfaces/Accelerometer.h"
#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Built-in accelerometer.
 *
 * This class allows access to the RoboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public Accelerometer,
                             public SensorBase,
                             public LiveWindowSendable {
 public:
  BuiltInAccelerometer(Range range = kRange_8G);
  virtual ~BuiltInAccelerometer();

  // Accelerometer interface
  virtual void SetRange(Range range) override;
  virtual double GetX() override;
  virtual double GetY() override;
  virtual double GetZ() override;

  virtual std::string GetSmartDashboardType() const override;
  virtual void InitTable(ITable *subtable) override;
  virtual void UpdateTable() override;
  virtual ITable *GetTable() const override;
  virtual void StartLiveWindowMode() override {}
  virtual void StopLiveWindowMode() override {}

 private:
  ITable *m_table;
};
