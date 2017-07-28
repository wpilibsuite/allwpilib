/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"
#include "interfaces/Accelerometer.h"

namespace frc {

/**
 * Built-in accelerometer.
 *
 * This class allows access to the roboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public Accelerometer,
                             public SensorBase,
                             public LiveWindowSendable {
 public:
  explicit BuiltInAccelerometer(Range range = kRange_8G);
  virtual ~BuiltInAccelerometer() = default;

  // Accelerometer interface
  void SetRange(Range range) override;
  double GetX() override;
  double GetY() override;
  double GetZ() override;

  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subtable) override;
  void UpdateTable() override;
  std::shared_ptr<ITable> GetTable() const override;
  void StartLiveWindowMode() override {}
  void StopLiveWindowMode() override {}

 private:
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
