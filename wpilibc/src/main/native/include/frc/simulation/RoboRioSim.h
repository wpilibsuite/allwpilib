/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <hal/simulation/RoboRioData.h>

#include "CallbackStore.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated RoboRIO.
 */
class RoboRioSim {
 public:
  static std::unique_ptr<CallbackStore> RegisterFPGAButtonCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioFPGAButtonCallback);
    store->SetUid(HALSIM_RegisterRoboRioFPGAButtonCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetFPGAButton() { return HALSIM_GetRoboRioFPGAButton(); }

  static void SetFPGAButton(bool fPGAButton) {
    HALSIM_SetRoboRioFPGAButton(fPGAButton);
  }

  static std::unique_ptr<CallbackStore> RegisterVInVoltageCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioVInVoltageCallback);
    store->SetUid(HALSIM_RegisterRoboRioVInVoltageCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetVInVoltage() { return HALSIM_GetRoboRioVInVoltage(); }

  static void SetVInVoltage(double vInVoltage) {
    HALSIM_SetRoboRioVInVoltage(vInVoltage);
  }

  static std::unique_ptr<CallbackStore> RegisterVInCurrentCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioVInCurrentCallback);
    store->SetUid(HALSIM_RegisterRoboRioVInCurrentCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetVInCurrent() { return HALSIM_GetRoboRioVInCurrent(); }

  static void SetVInCurrent(double vInCurrent) {
    HALSIM_SetRoboRioVInCurrent(vInCurrent);
  }

  static std::unique_ptr<CallbackStore> RegisterUserVoltage6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserVoltage6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage6VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserVoltage6V() { return HALSIM_GetRoboRioUserVoltage6V(); }

  static void SetUserVoltage6V(double userVoltage6V) {
    HALSIM_SetRoboRioUserVoltage6V(userVoltage6V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserCurrent6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserCurrent6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent6VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserCurrent6V() { return HALSIM_GetRoboRioUserCurrent6V(); }

  static void SetUserCurrent6V(double userCurrent6V) {
    HALSIM_SetRoboRioUserCurrent6V(userCurrent6V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserActive6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserActive6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive6VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetUserActive6V() { return HALSIM_GetRoboRioUserActive6V(); }

  static void SetUserActive6V(bool userActive6V) {
    HALSIM_SetRoboRioUserActive6V(userActive6V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserVoltage5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserVoltage5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage5VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserVoltage5V() { return HALSIM_GetRoboRioUserVoltage5V(); }

  static void SetUserVoltage5V(double userVoltage5V) {
    HALSIM_SetRoboRioUserVoltage5V(userVoltage5V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserCurrent5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserCurrent5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent5VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserCurrent5V() { return HALSIM_GetRoboRioUserCurrent5V(); }

  static void SetUserCurrent5V(double userCurrent5V) {
    HALSIM_SetRoboRioUserCurrent5V(userCurrent5V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserActive5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserActive5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive5VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetUserActive5V() { return HALSIM_GetRoboRioUserActive5V(); }

  static void SetUserActive5V(bool userActive5V) {
    HALSIM_SetRoboRioUserActive5V(userActive5V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserVoltage3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserVoltage3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserVoltage3V3Callback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserVoltage3V3() {
    return HALSIM_GetRoboRioUserVoltage3V3();
  }

  static void SetUserVoltage3V3(double userVoltage3V3) {
    HALSIM_SetRoboRioUserVoltage3V3(userVoltage3V3);
  }

  static std::unique_ptr<CallbackStore> RegisterUserCurrent3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserCurrent3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserCurrent3V3Callback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static double GetUserCurrent3V3() {
    return HALSIM_GetRoboRioUserCurrent3V3();
  }

  static void SetUserCurrent3V3(double userCurrent3V3) {
    HALSIM_SetRoboRioUserCurrent3V3(userCurrent3V3);
  }

  static std::unique_ptr<CallbackStore> RegisterUserActive3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserActive3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserActive3V3Callback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetUserActive3V3() { return HALSIM_GetRoboRioUserActive3V3(); }

  static void SetUserActive3V3(bool userActive3V3) {
    HALSIM_SetRoboRioUserActive3V3(userActive3V3);
  }

  static std::unique_ptr<CallbackStore> RegisterUserFaults6VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserFaults6VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults6VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static int GetUserFaults6V() { return HALSIM_GetRoboRioUserFaults6V(); }

  static void SetUserFaults6V(int userFaults6V) {
    HALSIM_SetRoboRioUserFaults6V(userFaults6V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserFaults5VCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserFaults5VCallback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults5VCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static int GetUserFaults5V() { return HALSIM_GetRoboRioUserFaults5V(); }

  static void SetUserFaults5V(int userFaults5V) {
    HALSIM_SetRoboRioUserFaults5V(userFaults5V);
  }

  static std::unique_ptr<CallbackStore> RegisterUserFaults3V3Callback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelRoboRioUserFaults3V3Callback);
    store->SetUid(HALSIM_RegisterRoboRioUserFaults3V3Callback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static int GetUserFaults3V3() { return HALSIM_GetRoboRioUserFaults3V3(); }

  static void SetUserFaults3V3(int userFaults3V3) {
    HALSIM_SetRoboRioUserFaults3V3(userFaults3V3);
  }

  static void ResetData() { HALSIM_ResetRoboRioData(); }
};
}  // namespace sim
}  // namespace frc
