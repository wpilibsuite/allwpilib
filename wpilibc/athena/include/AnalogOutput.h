/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"

/**
 * MXP analog output class.
 */
class AnalogOutput : public SensorBase, public LiveWindowSendable {
 public:
  explicit AnalogOutput(uint32_t channel);
  virtual ~AnalogOutput();

  void SetVoltage(float voltage);
  float GetVoltage() const;

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 protected:
  uint32_t m_channel;
  void* m_port;

  std::shared_ptr<ITable> m_table;
};
