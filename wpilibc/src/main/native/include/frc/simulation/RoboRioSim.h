// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <units/current.h>
#include <units/temperature.h>
#include <units/voltage.h>

#include "frc/RobotController.h"
#include "frc/simulation/CallbackStore.h"

namespace frc::sim {

/**
 * A utility class to control a simulated RoboRIO.
 */
class RoboRioSim {
 public:
  /**
   * Register a callback to be run whenever the Vin voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterVInVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the Vin voltage.
   *
   * @return the Vin voltage
   */
  static units::volt_t GetVInVoltage();

  /**
   * Define the Vin voltage.
   *
   * @param vInVoltage the new voltage
   */
  static void SetVInVoltage(units::volt_t vInVoltage);

  /**
   * Register a callback to be run whenever the 3.3V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterUserVoltage3V3Callback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 3.3V rail voltage.
   *
   * @return the 3.3V rail voltage
   */
  static units::volt_t GetUserVoltage3V3();

  /**
   * Define the 3.3V rail voltage.
   *
   * @param userVoltage3V3 the new voltage
   */
  static void SetUserVoltage3V3(units::volt_t userVoltage3V3);

  /**
   * Register a callback to be run whenever the 3.3V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterUserCurrent3V3Callback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 3.3V rail current.
   *
   * @return the 3.3V rail current
   */
  static units::ampere_t GetUserCurrent3V3();

  /**
   * Define the 3.3V rail current.
   *
   * @param userCurrent3V3 the new current
   */
  static void SetUserCurrent3V3(units::ampere_t userCurrent3V3);

  /**
   * Register a callback to be run whenever the 3.3V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterUserActive3V3Callback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the 3.3V rail active state.
   *
   * @return true if the 3.3V rail is active
   */
  static bool GetUserActive3V3();

  /**
   * Set the 3.3V rail active state.
   *
   * @param userActive3V3 true to make rail active
   */
  static void SetUserActive3V3(bool userActive3V3);

  /**
   * Register a callback to be run whenever the 3.3V rail number of faults
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterUserFaults3V3Callback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the 3.3V rail number of faults.
   *
   * @return number of faults
   */
  static int GetUserFaults3V3();

  /**
   * Set the 3.3V rail number of faults.
   *
   * @param userFaults3V3 number of faults
   */
  static void SetUserFaults3V3(int userFaults3V3);

  /**
   * Register a callback to be run whenever the brownout voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterBrownoutVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Measure the brownout voltage.
   *
   * @return the brownout voltage
   */
  static units::volt_t GetBrownoutVoltage();

  /**
   * Define the brownout voltage.
   *
   * @param brownoutVoltage the new voltage
   */
  static void SetBrownoutVoltage(units::volt_t brownoutVoltage);

  /**
   * Register a callback to be run whenever the cpu temp changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterCPUTempCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the cpu temp.
   *
   * @return the cpu temp.
   */
  static units::celsius_t GetCPUTemp();

  /**
   * Define the cpu temp.
   *
   * @param cpuTemp the new cpu temp.
   */
  static void SetCPUTemp(units::celsius_t cpuTemp);

  /**
   * Register a callback to be run whenever the team number changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterTeamNumberCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the team number.
   *
   * @return the team number.
   */
  static int32_t GetTeamNumber();

  /**
   * Set the team number.
   *
   * @param teamNumber the new team number.
   */
  static void SetTeamNumber(int32_t teamNumber);

  /**
   * Get the serial number.
   *
   * @return The serial number.
   */
  static std::string GetSerialNumber();

  /**
   * Set the serial number.
   *
   * @param serialNumber The serial number.
   */
  static void SetSerialNumber(std::string_view serialNumber);

  /**
   * Get the comments.
   *
   * @return The comments.
   */
  static std::string GetComments();

  /**
   * Set the comments.
   *
   * @param comments The comments.
   */
  static void SetComments(std::string_view comments);

  /**
   * Reset all simulation data.
   */
  static void ResetData();
};
}  // namespace frc::sim
