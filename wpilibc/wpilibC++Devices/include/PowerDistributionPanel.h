/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once
#ifndef __WPILIB_POWER_DISTRIBUTION_PANEL_H__
#define __WPILIB_POWER_DISTRIBUTION_PANEL_H__

#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 * The PDP must be at CAN Address 0.
 * @author Thomas Clark
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
  void InitTable(ITable *subTable) override;
  ITable *GetTable() const override;

 private:
  ITable *m_table = nullptr;
  uint8_t m_module;
};

#endif /* __WPILIB_POWER_DISTRIBUTION_PANEL_H__ */
