// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <units/current.h>
#include <units/voltage.h>

#include "frc/simulation/CallbackStore.h"

namespace frc::sim {

/**
 * Class to control a simulated RoboRIO.
 */
class RoboRioSim {
 public:
  /**
   * Register a callback to be run when the FPGA button state changes.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterFPGAButtonCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Query the state of the FPGA button.
   *
   * @return the FPGA button state
   */
  static bool GetFPGAButton();

  /**
   * Define the state of the FPGA button.
   *
   * @param fpgaButton the new state
   */
  static void SetFPGAButton(bool fPGAButton);

  /**
   * Register a callback to be run whenever the Vin voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether to call the callback with the initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterVInVoltageCallback(NotifyCallback callback, bool initialNotify);

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
   * Register a callback to be run whenever the Vin current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterVInCurrentCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Measure the Vin current.
   *
   * @return the Vin current
   */
  static units::ampere_t GetVInCurrent();

  /**
   * Define the Vin current.
   *
   * @param vInCurrent the new current
   */
  static void SetVInCurrent(units::ampere_t vInCurrent);

  /**
   * Register a callback to be run whenever the 6V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserVoltage6VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 6V rail voltage.
   *
   * @return the 6V rail voltage
   */
  static units::volt_t GetUserVoltage6V();

  /**
   * Define the 6V rail voltage.
   *
   * @param userVoltage6V the new voltage
   */
  static void SetUserVoltage6V(units::volt_t userVoltage6V);

  /**
   * Register a callback to be run whenever the 6V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserCurrent6VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 6V rail current.
   *
   * @return the 6V rail current
   */
  static units::ampere_t GetUserCurrent6V();

  /**
   * Define the 6V rail current.
   *
   * @param userCurrent6V the new current
   */
  static void SetUserCurrent6V(units::ampere_t userCurrent6V);

  /**
   * Register a callback to be run whenever the 6V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserActive6VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Get the 6V rail active state.
   *
   * @return true if the 6V rail is active
   */
  static bool GetUserActive6V();

  /**
   * Set the 6V rail active state.
   *
   * @param userActive6V true to make rail active
   */
  static void SetUserActive6V(bool userActive6V);

  /**
   * Register a callback to be run whenever the 5V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserVoltage5VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 5V rail voltage.
   *
   * @return the 5V rail voltage
   */
  static units::volt_t GetUserVoltage5V();

  /**
   * Define the 5V rail voltage.
   *
   * @param userVoltage5V the new voltage
   */
  static void SetUserVoltage5V(units::volt_t userVoltage5V);

  /**
   * Register a callback to be run whenever the 5V rail current changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserCurrent5VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Measure the 5V rail current.
   *
   * @return the 5V rail current
   */
  static units::ampere_t GetUserCurrent5V();

  /**
   * Define the 5V rail current.
   *
   * @param userCurrent5V the new current
   */
  static void SetUserCurrent5V(units::ampere_t userCurrent5V);

  /**
   * Register a callback to be run whenever the 5V rail active state changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial state
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserActive5VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Get the 5V rail active state.
   *
   * @return true if the 5V rail is active
   */
  static bool GetUserActive5V();

  /**
   * Set the 5V rail active state.
   *
   * @param userActive5V true to make rail active
   */
  static void SetUserActive5V(bool userActive5V);

  /**
   * Register a callback to be run whenever the 3.3V rail voltage changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserVoltage3V3Callback(NotifyCallback callback, bool initialNotify);

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
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserCurrent3V3Callback(NotifyCallback callback, bool initialNotify);

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
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserActive3V3Callback(NotifyCallback callback, bool initialNotify);

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
   * Register a callback to be run whenever the 6V rail number of faults
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserFaults6VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Get the 6V rail number of faults.
   *
   * @return number of faults
   */
  static int GetUserFaults6V();

  /**
   * Set the 6V rail number of faults.
   *
   * @param userFaults6V number of faults
   */
  static void SetUserFaults6V(int userFaults6V);

  /**
   * Register a callback to be run whenever the 5V rail number of faults
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserFaults5VCallback(NotifyCallback callback, bool initialNotify);

  /**
   * Get the 5V rail number of faults.
   *
   * @return number of faults
   */
  static int GetUserFaults5V();

  /**
   * Set the 5V rail number of faults.
   *
   * @param userFaults5V number of faults
   */
  static void SetUserFaults5V(int userFaults5V);

  /**
   * Register a callback to be run whenever the 3.3V rail number of faults
   * changes.
   *
   * @param callback the callback
   * @param initialNotify whether the callback should be called with the
   *                      initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] static std::unique_ptr<CallbackStore>
  RegisterUserFaults3V3Callback(NotifyCallback callback, bool initialNotify);

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
   * Reset all simulation data.
   */
  static void ResetData();
};
}  // namespace frc::sim
