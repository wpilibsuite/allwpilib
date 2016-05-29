/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 */
class PowerDistributionPanel : public SensorBase, public LiveWindowSendable {
 public:
  PowerDistributionPanel();
  PowerDistributionPanel(uint8_t module);

  double GetVoltage() const;
  double GetTemperature() const;
  double GetCurrent(uint8_t channel) const;
  double GetTotalCurrent() const;
  double GetTotalPower() const;
  double GetTotalEnergy() const;
  void ResetTotalEnergy();
  void ClearStickyFaults();

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 private:
  std::shared_ptr<ITable> m_table;
  uint8_t m_module;
};
