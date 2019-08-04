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
#include "mockdata/AnalogInData.h"

namespace frc {
namespace sim {
class AnalogInSim {
 public:
  explicit AnalogInSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInInitializedCallback);
    store->SetUid(HALSIM_RegisterAnalogInInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const { return HALSIM_GetAnalogInInitialized(m_index); }

  void SetInitialized(bool initialized) {
    HALSIM_SetAnalogInInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterAverageBitsCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInAverageBitsCallback);
    store->SetUid(HALSIM_RegisterAnalogInAverageBitsCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetAverageBits() const { return HALSIM_GetAnalogInAverageBits(m_index); }

  void SetAverageBits(int averageBits) {
    HALSIM_SetAnalogInAverageBits(m_index, averageBits);
  }

  std::unique_ptr<CallbackStore> RegisterOversampleBitsCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInOversampleBitsCallback);
    store->SetUid(HALSIM_RegisterAnalogInOversampleBitsCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetOversampleBits() const {
    return HALSIM_GetAnalogInOversampleBits(m_index);
  }

  void SetOversampleBits(int oversampleBits) {
    HALSIM_SetAnalogInOversampleBits(m_index, oversampleBits);
  }

  std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInVoltageCallback);
    store->SetUid(HALSIM_RegisterAnalogInVoltageCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetVoltage() const { return HALSIM_GetAnalogInVoltage(m_index); }

  void SetVoltage(double voltage) {
    HALSIM_SetAnalogInVoltage(m_index, voltage);
  }

  std::unique_ptr<CallbackStore> RegisterAccumulatorInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback,
        &HALSIM_CancelAnalogInAccumulatorInitializedCallback);
    store->SetUid(HALSIM_RegisterAnalogInAccumulatorInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetAccumulatorInitialized() const {
    return HALSIM_GetAnalogInAccumulatorInitialized(m_index);
  }

  void SetAccumulatorInitialized(bool accumulatorInitialized) {
    HALSIM_SetAnalogInAccumulatorInitialized(m_index, accumulatorInitialized);
  }

  std::unique_ptr<CallbackStore> RegisterAccumulatorValueCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInAccumulatorValueCallback);
    store->SetUid(HALSIM_RegisterAnalogInAccumulatorValueCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int64_t GetAccumulatorValue() const {
    return HALSIM_GetAnalogInAccumulatorValue(m_index);
  }

  void SetAccumulatorValue(int64_t accumulatorValue) {
    HALSIM_SetAnalogInAccumulatorValue(m_index, accumulatorValue);
  }

  std::unique_ptr<CallbackStore> RegisterAccumulatorCountCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInAccumulatorCountCallback);
    store->SetUid(HALSIM_RegisterAnalogInAccumulatorCountCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int64_t GetAccumulatorCount() const {
    return HALSIM_GetAnalogInAccumulatorCount(m_index);
  }

  void SetAccumulatorCount(int64_t accumulatorCount) {
    HALSIM_SetAnalogInAccumulatorCount(m_index, accumulatorCount);
  }

  std::unique_ptr<CallbackStore> RegisterAccumulatorCenterCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogInAccumulatorCenterCallback);
    store->SetUid(HALSIM_RegisterAnalogInAccumulatorCenterCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetAccumulatorCenter() const {
    return HALSIM_GetAnalogInAccumulatorCenter(m_index);
  }

  void SetAccumulatorCenter(int accumulatorCenter) {
    HALSIM_SetAnalogInAccumulatorCenter(m_index, accumulatorCenter);
  }

  std::unique_ptr<CallbackStore> RegisterAccumulatorDeadbandCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback,
        &HALSIM_CancelAnalogInAccumulatorDeadbandCallback);
    store->SetUid(HALSIM_RegisterAnalogInAccumulatorDeadbandCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetAccumulatorDeadband() const {
    return HALSIM_GetAnalogInAccumulatorDeadband(m_index);
  }

  void SetAccumulatorDeadband(int accumulatorDeadband) {
    HALSIM_SetAnalogInAccumulatorDeadband(m_index, accumulatorDeadband);
  }

  void ResetData() { HALSIM_ResetAnalogInData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
#endif  // __FRC_ROBORIO__
