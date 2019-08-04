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
#include "mockdata/EncoderData.h"

namespace frc {
namespace sim {
class EncoderSim {
 public:
  explicit EncoderSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderInitializedCallback);
    store->SetUid(HALSIM_RegisterEncoderInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const { return HALSIM_GetEncoderInitialized(m_index); }

  void SetInitialized(bool initialized) {
    HALSIM_SetEncoderInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterCountCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderCountCallback);
    store->SetUid(HALSIM_RegisterEncoderCountCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetCount() const { return HALSIM_GetEncoderCount(m_index); }

  void SetCount(int count) { HALSIM_SetEncoderCount(m_index, count); }

  std::unique_ptr<CallbackStore> RegisterPeriodCallback(NotifyCallback callback,
                                                        bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderPeriodCallback);
    store->SetUid(HALSIM_RegisterEncoderPeriodCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetPeriod() const { return HALSIM_GetEncoderPeriod(m_index); }

  void SetPeriod(double period) { HALSIM_SetEncoderPeriod(m_index, period); }

  std::unique_ptr<CallbackStore> RegisterResetCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderResetCallback);
    store->SetUid(HALSIM_RegisterEncoderResetCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetReset() const { return HALSIM_GetEncoderReset(m_index); }

  void SetReset(bool reset) { HALSIM_SetEncoderReset(m_index, reset); }

  std::unique_ptr<CallbackStore> RegisterMaxPeriodCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderMaxPeriodCallback);
    store->SetUid(HALSIM_RegisterEncoderMaxPeriodCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetMaxPeriod() const { return HALSIM_GetEncoderMaxPeriod(m_index); }

  void SetMaxPeriod(double maxPeriod) {
    HALSIM_SetEncoderMaxPeriod(m_index, maxPeriod);
  }

  std::unique_ptr<CallbackStore> RegisterDirectionCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderDirectionCallback);
    store->SetUid(HALSIM_RegisterEncoderDirectionCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetDirection() const { return HALSIM_GetEncoderDirection(m_index); }

  void SetDirection(bool direction) {
    HALSIM_SetEncoderDirection(m_index, direction);
  }

  std::unique_ptr<CallbackStore> RegisterReverseDirectionCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderReverseDirectionCallback);
    store->SetUid(HALSIM_RegisterEncoderReverseDirectionCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetReverseDirection() const {
    return HALSIM_GetEncoderReverseDirection(m_index);
  }

  void SetReverseDirection(bool reverseDirection) {
    HALSIM_SetEncoderReverseDirection(m_index, reverseDirection);
  }

  std::unique_ptr<CallbackStore> RegisterSamplesToAverageCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderSamplesToAverageCallback);
    store->SetUid(HALSIM_RegisterEncoderSamplesToAverageCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetSamplesToAverage() const {
    return HALSIM_GetEncoderSamplesToAverage(m_index);
  }

  void SetSamplesToAverage(int samplesToAverage) {
    HALSIM_SetEncoderSamplesToAverage(m_index, samplesToAverage);
  }

  std::unique_ptr<CallbackStore> RegisterDistancePerPulseCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelEncoderDistancePerPulseCallback);
    store->SetUid(HALSIM_RegisterEncoderDistancePerPulseCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetDistancePerPulse() const {
    return HALSIM_GetEncoderDistancePerPulse(m_index);
  }

  void SetDistancePerPulse(double distancePerPulse) {
    HALSIM_SetEncoderDistancePerPulse(m_index, distancePerPulse);
  }

  void ResetData() { HALSIM_ResetEncoderData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
#endif  // __FRC_ROBORIO__
