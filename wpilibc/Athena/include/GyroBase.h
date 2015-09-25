/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SensorBase.h"
#include "PIDSource.h"
#include "interfaces/Gyro.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

/**
 * GyroBase is the common base class for Gyro implementations such as
 * AnalogGyro.
 */
class GyroBase : public Gyro, public SensorBase, public PIDSource, public LiveWindowSendable {
 public:
  virtual ~GyroBase() = default;

  // PIDSource interface
  double PIDGet() override;

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 private:
  std::shared_ptr<ITable> m_table;
};
