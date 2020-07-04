/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <hal/simulation/DriverStationData.h>

#include "CallbackStore.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated driver station.
 */
class DriverStationSim {
 public:
  static std::unique_ptr<CallbackStore> RegisterEnabledCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEnabledCallback);
    store->SetUid(HALSIM_RegisterDriverStationEnabledCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetEnabled() { return HALSIM_GetDriverStationEnabled(); }

  static void SetEnabled(bool enabled) {
    HALSIM_SetDriverStationEnabled(enabled);
  }

  static std::unique_ptr<CallbackStore> RegisterAutonomousCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationAutonomousCallback);
    store->SetUid(HALSIM_RegisterDriverStationAutonomousCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetAutonomous() { return HALSIM_GetDriverStationAutonomous(); }

  static void SetAutonomous(bool autonomous) {
    HALSIM_SetDriverStationAutonomous(autonomous);
  }

  static std::unique_ptr<CallbackStore> RegisterTestCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationTestCallback);
    store->SetUid(HALSIM_RegisterDriverStationTestCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetTest() { return HALSIM_GetDriverStationTest(); }

  static void SetTest(bool test) { HALSIM_SetDriverStationTest(test); }

  static std::unique_ptr<CallbackStore> RegisterEStopCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEStopCallback);
    store->SetUid(HALSIM_RegisterDriverStationEStopCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetEStop() { return HALSIM_GetDriverStationEStop(); }

  static void SetEStop(bool eStop) { HALSIM_SetDriverStationEStop(eStop); }

  static std::unique_ptr<CallbackStore> RegisterFmsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationFmsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationFmsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetFmsAttached() { return HALSIM_GetDriverStationFmsAttached(); }

  static void SetFmsAttached(bool fmsAttached) {
    HALSIM_SetDriverStationFmsAttached(fmsAttached);
  }

  static std::unique_ptr<CallbackStore> RegisterDsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationDsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationDsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  static bool GetDsAttached() { return HALSIM_GetDriverStationDsAttached(); }

  static void SetDsAttached(bool dsAttached) {
    HALSIM_SetDriverStationDsAttached(dsAttached);
  }

  static void NotifyNewData() { HALSIM_NotifyDriverStationNewData(); }

  static void ResetData() { HALSIM_ResetDriverStationData(); }
};
}  // namespace sim
}  // namespace frc
