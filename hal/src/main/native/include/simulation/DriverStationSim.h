/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include <memory>
#include <utility>

#include "CallbackStore.h"
#include "mockdata/DriverStationData.h"

namespace frc {
namespace sim {
class DriverStationSim {
 public:
  std::unique_ptr<CallbackStore> RegisterEnabledCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEnabledCallback);
    store->SetUid(HALSIM_RegisterDriverStationEnabledCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetEnabled() const { return HALSIM_GetDriverStationEnabled(); }

  void SetEnabled(bool enabled) { HALSIM_SetDriverStationEnabled(enabled); }

  std::unique_ptr<CallbackStore> RegisterAutonomousCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationAutonomousCallback);
    store->SetUid(HALSIM_RegisterDriverStationAutonomousCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetAutonomous() const { return HALSIM_GetDriverStationAutonomous(); }

  void SetAutonomous(bool autonomous) {
    HALSIM_SetDriverStationAutonomous(autonomous);
  }

  std::unique_ptr<CallbackStore> RegisterTestCallback(NotifyCallback callback,
                                                      bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationTestCallback);
    store->SetUid(HALSIM_RegisterDriverStationTestCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetTest() const { return HALSIM_GetDriverStationTest(); }

  void SetTest(bool test) { HALSIM_SetDriverStationTest(test); }

  std::unique_ptr<CallbackStore> RegisterEStopCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationEStopCallback);
    store->SetUid(HALSIM_RegisterDriverStationEStopCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetEStop() const { return HALSIM_GetDriverStationEStop(); }

  void SetEStop(bool eStop) { HALSIM_SetDriverStationEStop(eStop); }

  std::unique_ptr<CallbackStore> RegisterFmsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationFmsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationFmsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetFmsAttached() const { return HALSIM_GetDriverStationFmsAttached(); }

  void SetFmsAttached(bool fmsAttached) {
    HALSIM_SetDriverStationFmsAttached(fmsAttached);
  }

  std::unique_ptr<CallbackStore> RegisterDsAttachedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        -1, callback, &HALSIM_CancelDriverStationDsAttachedCallback);
    store->SetUid(HALSIM_RegisterDriverStationDsAttachedCallback(
        &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetDsAttached() const { return HALSIM_GetDriverStationDsAttached(); }

  void SetDsAttached(bool dsAttached) {
    HALSIM_SetDriverStationDsAttached(dsAttached);
  }

  void NotifyNewData() { HALSIM_NotifyDriverStationNewData(); }

  void ResetData() { HALSIM_ResetDriverStationData(); }
};
}  // namespace sim
}  // namespace frc
#endif  // __FRC_ROBORIO__
