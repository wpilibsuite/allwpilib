/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "LiveWindow/LiveWindowSendable.h"
#include "PIDSource.h"
#include "SensorBase.h"
#include "interfaces/Gyro.h"

namespace frc {

/**
 * GyroBase is the common base class for Gyro implementations such as
 * AnalogGyro.
 */
class GyroBase : public Gyro,
                 public SensorBase,
                 public PIDSource,
                 public LiveWindowSendable {
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

}  // namespace frc
