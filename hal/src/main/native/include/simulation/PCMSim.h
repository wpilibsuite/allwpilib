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
#include "mockdata/PCMData.h"

namespace frc {
namespace sim {
class PCMSim {
 public:
  explicit PCMSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterSolenoidInitializedCallback(
      int channel, NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, channel, -1, callback,
        &HALSIM_CancelPCMSolenoidInitializedCallback);
    store->SetUid(HALSIM_RegisterPCMSolenoidInitializedCallback(
        m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetSolenoidInitialized(int channel) const {
    return HALSIM_GetPCMSolenoidInitialized(m_index, channel);
  }

  void SetSolenoidInitialized(int channel, bool solenoidInitialized) {
    HALSIM_SetPCMSolenoidInitialized(m_index, channel, solenoidInitialized);
  }

  std::unique_ptr<CallbackStore> RegisterSolenoidOutputCallback(
      int channel, NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, channel, -1, callback,
        &HALSIM_CancelPCMSolenoidOutputCallback);
    store->SetUid(HALSIM_RegisterPCMSolenoidOutputCallback(
        m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetSolenoidOutput(int channel) const {
    return HALSIM_GetPCMSolenoidOutput(m_index, channel);
  }

  void SetSolenoidOutput(int channel, bool solenoidOutput) {
    HALSIM_SetPCMSolenoidOutput(m_index, channel, solenoidOutput);
  }

  std::unique_ptr<CallbackStore> RegisterCompressorInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPCMCompressorInitializedCallback);
    store->SetUid(HALSIM_RegisterPCMCompressorInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetCompressorInitialized() const {
    return HALSIM_GetPCMCompressorInitialized(m_index);
  }

  void SetCompressorInitialized(bool compressorInitialized) {
    HALSIM_SetPCMCompressorInitialized(m_index, compressorInitialized);
  }

  std::unique_ptr<CallbackStore> RegisterCompressorOnCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPCMCompressorOnCallback);
    store->SetUid(HALSIM_RegisterPCMCompressorOnCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetCompressorOn() const { return HALSIM_GetPCMCompressorOn(m_index); }

  void SetCompressorOn(bool compressorOn) {
    HALSIM_SetPCMCompressorOn(m_index, compressorOn);
  }

  std::unique_ptr<CallbackStore> RegisterClosedLoopEnabledCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPCMClosedLoopEnabledCallback);
    store->SetUid(HALSIM_RegisterPCMClosedLoopEnabledCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetClosedLoopEnabled() const {
    return HALSIM_GetPCMClosedLoopEnabled(m_index);
  }

  void SetClosedLoopEnabled(bool closedLoopEnabled) {
    HALSIM_SetPCMClosedLoopEnabled(m_index, closedLoopEnabled);
  }

  std::unique_ptr<CallbackStore> RegisterPressureSwitchCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPCMPressureSwitchCallback);
    store->SetUid(HALSIM_RegisterPCMPressureSwitchCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetPressureSwitch() const {
    return HALSIM_GetPCMPressureSwitch(m_index);
  }

  void SetPressureSwitch(bool pressureSwitch) {
    HALSIM_SetPCMPressureSwitch(m_index, pressureSwitch);
  }

  std::unique_ptr<CallbackStore> RegisterCompressorCurrentCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelPCMCompressorCurrentCallback);
    store->SetUid(HALSIM_RegisterPCMCompressorCurrentCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetCompressorCurrent() const {
    return HALSIM_GetPCMCompressorCurrent(m_index);
  }

  void SetCompressorCurrent(double compressorCurrent) {
    HALSIM_SetPCMCompressorCurrent(m_index, compressorCurrent);
  }

  uint8_t GetAllSolenoidOutputs() {
    uint8_t ret = 0;
    HALSIM_GetPCMAllSolenoids(m_index, &ret);
    return ret;
  }

  void SetAllSolenoidOutputs(uint8_t outputs) {
    HALSIM_SetPCMAllSolenoids(m_index, outputs);
  }

  void ResetData() { HALSIM_ResetPCMData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
