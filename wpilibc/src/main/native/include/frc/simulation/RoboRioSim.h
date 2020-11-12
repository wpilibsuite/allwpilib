/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <units/current.h>
#include <units/voltage.h>

#include "frc/simulation/CallbackStore.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated RoboRIO.
 */
class RoboRioSim {
 public:
  static std::unique_ptr<CallbackStore> RegisterFPGAButtonCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetFPGAButton();

  static void SetFPGAButton(bool fPGAButton);

  static std::unique_ptr<CallbackStore> RegisterVInVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  static units::volt_t GetVInVoltage();

  static void SetVInVoltage(units::volt_t vInVoltage);

  static std::unique_ptr<CallbackStore> RegisterVInCurrentCallback(
      NotifyCallback callback, bool initialNotify);

  static units::ampere_t GetVInCurrent();

  static void SetVInCurrent(units::ampere_t vInCurrent);

  static std::unique_ptr<CallbackStore> RegisterUserVoltage6VCallback(
      NotifyCallback callback, bool initialNotify);

  static units::volt_t GetUserVoltage6V();

  static void SetUserVoltage6V(units::volt_t userVoltage6V);

  static std::unique_ptr<CallbackStore> RegisterUserCurrent6VCallback(
      NotifyCallback callback, bool initialNotify);

  static units::ampere_t GetUserCurrent6V();

  static void SetUserCurrent6V(units::ampere_t userCurrent6V);

  static std::unique_ptr<CallbackStore> RegisterUserActive6VCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetUserActive6V();

  static void SetUserActive6V(bool userActive6V);

  static std::unique_ptr<CallbackStore> RegisterUserVoltage5VCallback(
      NotifyCallback callback, bool initialNotify);

  static units::volt_t GetUserVoltage5V();

  static void SetUserVoltage5V(units::volt_t userVoltage5V);

  static std::unique_ptr<CallbackStore> RegisterUserCurrent5VCallback(
      NotifyCallback callback, bool initialNotify);

  static units::ampere_t GetUserCurrent5V();

  static void SetUserCurrent5V(units::ampere_t userCurrent5V);

  static std::unique_ptr<CallbackStore> RegisterUserActive5VCallback(
      NotifyCallback callback, bool initialNotify);

  static bool GetUserActive5V();

  static void SetUserActive5V(bool userActive5V);

  static std::unique_ptr<CallbackStore> RegisterUserVoltage3V3Callback(
      NotifyCallback callback, bool initialNotify);

  static units::volt_t GetUserVoltage3V3();

  static void SetUserVoltage3V3(units::volt_t userVoltage3V3);

  static std::unique_ptr<CallbackStore> RegisterUserCurrent3V3Callback(
      NotifyCallback callback, bool initialNotify);

  static units::ampere_t GetUserCurrent3V3();

  static void SetUserCurrent3V3(units::ampere_t userCurrent3V3);

  static std::unique_ptr<CallbackStore> RegisterUserActive3V3Callback(
      NotifyCallback callback, bool initialNotify);

  static bool GetUserActive3V3();

  static void SetUserActive3V3(bool userActive3V3);

  static std::unique_ptr<CallbackStore> RegisterUserFaults6VCallback(
      NotifyCallback callback, bool initialNotify);

  static int GetUserFaults6V();

  static void SetUserFaults6V(int userFaults6V);

  static std::unique_ptr<CallbackStore> RegisterUserFaults5VCallback(
      NotifyCallback callback, bool initialNotify);

  static int GetUserFaults5V();

  static void SetUserFaults5V(int userFaults5V);

  static std::unique_ptr<CallbackStore> RegisterUserFaults3V3Callback(
      NotifyCallback callback, bool initialNotify);

  static int GetUserFaults3V3();

  static void SetUserFaults3V3(int userFaults3V3);

  static void ResetData();
};
}  // namespace sim
}  // namespace frc
