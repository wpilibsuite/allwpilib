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
#include "mockdata/DIOData.h"

namespace frc {
namespace sim {
class DIOSim {
 public:
  explicit DIOSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDIOInitializedCallback);
    store->SetUid(HALSIM_RegisterDIOInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const { return HALSIM_GetDIOInitialized(m_index); }

  void SetInitialized(bool initialized) {
    HALSIM_SetDIOInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterValueCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDIOValueCallback);
    store->SetUid(HALSIM_RegisterDIOValueCallback(m_index, &CallbackStoreThunk,
                                                  store.get(), initialNotify));
    return store;
  }

  bool GetValue() const { return HALSIM_GetDIOValue(m_index); }

  void SetValue(bool value) { HALSIM_SetDIOValue(m_index, value); }

  std::unique_ptr<CallbackStore> RegisterPulseLengthCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDIOPulseLengthCallback);
    store->SetUid(HALSIM_RegisterDIOPulseLengthCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetPulseLength() const { return HALSIM_GetDIOPulseLength(m_index); }

  void SetPulseLength(double pulseLength) {
    HALSIM_SetDIOPulseLength(m_index, pulseLength);
  }

  std::unique_ptr<CallbackStore> RegisterIsInputCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDIOIsInputCallback);
    store->SetUid(HALSIM_RegisterDIOIsInputCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetIsInput() const { return HALSIM_GetDIOIsInput(m_index); }

  void SetIsInput(bool isInput) { HALSIM_SetDIOIsInput(m_index, isInput); }

  std::unique_ptr<CallbackStore> RegisterFilterIndexCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDIOFilterIndexCallback);
    store->SetUid(HALSIM_RegisterDIOFilterIndexCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetFilterIndex() const { return HALSIM_GetDIOFilterIndex(m_index); }

  void SetFilterIndex(int filterIndex) {
    HALSIM_SetDIOFilterIndex(m_index, filterIndex);
  }

  void ResetData() { HALSIM_ResetDIOData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
