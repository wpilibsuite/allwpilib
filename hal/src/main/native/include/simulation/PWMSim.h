/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include "CallbackStore.h"
#include "mockdata/PWMData.h"

namespace frc {
namespace sim {
class PWMSim {
 public:
  explicit PWMSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMInitializedCallback);
    store->SetUid(HALSIM_RegisterPWMInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const { return HALSIM_GetPWMInitialized(m_index); }

  void SetInitialized(bool initialized) {
    HALSIM_SetPWMInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterRawValueCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMRawValueCallback);
    store->SetUid(HALSIM_RegisterPWMRawValueCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetRawValue() const { return HALSIM_GetPWMRawValue(m_index); }

  void SetRawValue(int rawValue) { HALSIM_SetPWMRawValue(m_index, rawValue); }

  std::unique_ptr<CallbackStore> RegisterSpeedCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMSpeedCallback);
    store->SetUid(HALSIM_RegisterPWMSpeedCallback(m_index, &CallbackStoreThunk,
                                                  store.get(), initialNotify));
    return store;
  }

  double GetSpeed() const { return HALSIM_GetPWMSpeed(m_index); }

  void SetSpeed(double speed) { HALSIM_SetPWMSpeed(m_index, speed); }

  std::unique_ptr<CallbackStore> RegisterPositionCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMPositionCallback);
    store->SetUid(HALSIM_RegisterPWMPositionCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetPosition() const { return HALSIM_GetPWMPosition(m_index); }

  void SetPosition(double position) {
    HALSIM_SetPWMPosition(m_index, position);
  }

  std::unique_ptr<CallbackStore> RegisterPeriodScaleCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMPeriodScaleCallback);
    store->SetUid(HALSIM_RegisterPWMPeriodScaleCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetPeriodScale() const { return HALSIM_GetPWMPeriodScale(m_index); }

  void SetPeriodScale(int periodScale) {
    HALSIM_SetPWMPeriodScale(m_index, periodScale);
  }

  std::unique_ptr<CallbackStore> RegisterZeroLatchCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPWMZeroLatchCallback);
    store->SetUid(HALSIM_RegisterPWMZeroLatchCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetZeroLatch() const { return HALSIM_GetPWMZeroLatch(m_index); }

  void SetZeroLatch(bool zeroLatch) {
    HALSIM_SetPWMZeroLatch(m_index, zeroLatch);
  }

  void ResetData() { HALSIM_ResetPWMData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
