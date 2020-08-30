/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class PowerDistributionPanel;

namespace sim {

/**
 * Class to control a simulated Power Distribution Panel (PDP).
 */
class PDPSim {
 public:
  /**
   * Constructs from a PDP module number (CAN ID).
   *
   * @param module module number
   */
  explicit PDPSim(int module = 0);

  /**
   * Constructs from a PowerDistributionPanel object.
   *
   * @param pdp PowerDistributionPanel to simulate
   */
  explicit PDPSim(const PowerDistributionPanel& pdp);

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterTemperatureCallback(
      NotifyCallback callback, bool initialNotify);

  double GetTemperature() const;

  void SetTemperature(double temperature);

  std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  double GetVoltage() const;

  void SetVoltage(double voltage);

  std::unique_ptr<CallbackStore> RegisterCurrentCallback(
      int channel, NotifyCallback callback, bool initialNotify);

  double GetCurrent(int channel) const;

  void SetCurrent(int channel, double current);

  void GetAllCurrents(double* currents) const;

  void SetAllCurrents(const double* currents);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
