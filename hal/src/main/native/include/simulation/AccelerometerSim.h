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
#include "mockdata/AccelerometerData.h"

namespace frc {
namespace sim {
class AccelerometerSim {
 public:
  explicit AccelerometerSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterActiveCallback(NotifyCallback callback,
                                                        bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAccelerometerActiveCallback);
    store->SetUid(HALSIM_RegisterAccelerometerActiveCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetActive() const { return HALSIM_GetAccelerometerActive(m_index); }

  void SetActive(bool active) {
    HALSIM_SetAccelerometerActive(m_index, active);
  }

  std::unique_ptr<CallbackStore> RegisterRangeCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAccelerometerRangeCallback);
    store->SetUid(HALSIM_RegisterAccelerometerRangeCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  HAL_AccelerometerRange GetRange() const {
    return HALSIM_GetAccelerometerRange(m_index);
  }

  void SetRange(HAL_AccelerometerRange range) {
    HALSIM_SetAccelerometerRange(m_index, range);
  }

  std::unique_ptr<CallbackStore> RegisterXCallback(NotifyCallback callback,
                                                   bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAccelerometerXCallback);
    store->SetUid(HALSIM_RegisterAccelerometerXCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetX() const { return HALSIM_GetAccelerometerX(m_index); }

  void SetX(double x) { HALSIM_SetAccelerometerX(m_index, x); }

  std::unique_ptr<CallbackStore> RegisterYCallback(NotifyCallback callback,
                                                   bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAccelerometerYCallback);
    store->SetUid(HALSIM_RegisterAccelerometerYCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetY() const { return HALSIM_GetAccelerometerY(m_index); }

  void SetY(double y) { HALSIM_SetAccelerometerY(m_index, y); }

  std::unique_ptr<CallbackStore> RegisterZCallback(NotifyCallback callback,
                                                   bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAccelerometerZCallback);
    store->SetUid(HALSIM_RegisterAccelerometerZCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetZ() const { return HALSIM_GetAccelerometerZ(m_index); }

  void SetZ(double z) { HALSIM_SetAccelerometerZ(m_index, z); }

  void ResetData() { HALSIM_ResetAccelerometerData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
#endif  // __FRC_ROBORIO__
