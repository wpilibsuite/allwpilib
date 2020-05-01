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
#include "mockdata/DigitalPWMData.h"

namespace frc {
namespace sim {
class DigitalPWMSim {
 public:
  explicit DigitalPWMSim(int index) { m_index = index; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMInitializedCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetDigitalPWMInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetDigitalPWMInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterDutyCycleCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMDutyCycleCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMDutyCycleCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetDutyCycle() const { return HALSIM_GetDigitalPWMDutyCycle(m_index); }

  void SetDutyCycle(double dutyCycle) {
    HALSIM_SetDigitalPWMDutyCycle(m_index, dutyCycle);
  }

  std::unique_ptr<CallbackStore> RegisterPinCallback(NotifyCallback callback,
                                                     bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMPinCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMPinCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetPin() const { return HALSIM_GetDigitalPWMPin(m_index); }

  void SetPin(int pin) { HALSIM_SetDigitalPWMPin(m_index, pin); }

  void ResetData() { HALSIM_ResetDigitalPWMData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
