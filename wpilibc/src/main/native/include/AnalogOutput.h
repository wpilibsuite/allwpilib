/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <HAL/AnalogOutput.h>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"

namespace frc {

/**
 * MXP analog output class.
 */
class AnalogOutput : public SensorBase, public LiveWindowSendable {
 public:
  explicit AnalogOutput(int channel);
  virtual ~AnalogOutput();

  void SetVoltage(double voltage);
  double GetVoltage() const;
  int GetChannel();

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 protected:
  int m_channel;
  HAL_AnalogOutputHandle m_port;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
