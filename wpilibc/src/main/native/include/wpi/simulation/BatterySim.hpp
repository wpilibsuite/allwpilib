// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <numeric>
#include <span>

#include <units/current.h>
#include <units/impedance.h>
#include <units/voltage.h>

namespace frc::sim {

/**
 * A utility class to simulate the robot battery.
 */
class BatterySim {
 public:
  /**
   * Calculate the loaded battery voltage. Use this with
   * RoboRioSim::SetVInVoltage(double) to set the simulated battery voltage,
   * which can then be retrieved with the RobotController::GetBatteryVoltage()
   * method.
   *
   * @param nominalVoltage The nominal battery voltage. Usually 12v.
   * @param resistance     The forward resistance of the battery. Most batteries
   *                       are at or below 20 milliohms.
   * @param currents       The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  static units::volt_t Calculate(units::volt_t nominalVoltage,
                                 units::ohm_t resistance,
                                 std::span<const units::ampere_t> currents) {
    return std::max(0_V, nominalVoltage - std::accumulate(currents.begin(),
                                                          currents.end(), 0_A) *
                                              resistance);
  }

  /**
   * Calculate the loaded battery voltage. Use this with
   * RoboRioSim::SetVInVoltage(double) to set the simulated battery voltage,
   * which can then be retrieved with the RobotController::GetBatteryVoltage()
   * method.
   *
   * @param nominalVoltage The nominal battery voltage. Usually 12v.
   * @param resistance     The forward resistance of the battery. Most batteries
   *                       are at or below 20 milliohms.
   * @param currents       The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  static units::volt_t Calculate(
      units::volt_t nominalVoltage, units::ohm_t resistance,
      std::initializer_list<units::ampere_t> currents) {
    return std::max(0_V, nominalVoltage - std::accumulate(currents.begin(),
                                                          currents.end(), 0_A) *
                                              resistance);
  }

  /**
   * Calculate the loaded battery voltage. Use this with
   * RoboRioSimSetVInVoltage(double) to set the simulated battery voltage, which
   * can then be retrieved with the RobotController::GetBatteryVoltage() method.
   * This function assumes a nominal voltage of 12V and a resistance of 20
   * milliohms (0.020 ohms).
   *
   * @param currents The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  static units::volt_t Calculate(std::span<const units::ampere_t> currents) {
    return Calculate(12_V, 0.02_Ohm, currents);
  }

  /**
   * Calculate the loaded battery voltage. Use this with
   * RoboRioSimSetVInVoltage(double) to set the simulated battery voltage, which
   * can then be retrieved with the RobotController::GetBatteryVoltage() method.
   * This function assumes a nominal voltage of 12V and a resistance of 20
   * milliohms (0.020 ohms).
   *
   * @param currents The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  static units::volt_t Calculate(
      std::initializer_list<units::ampere_t> currents) {
    return Calculate(12_V, 0.02_Ohm, currents);
  }
};

}  // namespace frc::sim
