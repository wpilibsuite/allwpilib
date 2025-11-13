// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PWMSim.hpp"

#include <memory>

#include "wpi/hal/simulation/PWMData.h"
#include "wpi/hardware/discrete/PWM.hpp"
#include "wpi/hardware/motor/PWMMotorController.hpp"

using namespace wpi;
using namespace wpi::sim;

PWMSim::PWMSim(const PWM& pwm) : m_index{pwm.GetChannel()} {}

PWMSim::PWMSim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> PWMSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMInitializedCallback);
  store->SetUid(HALSIM_RegisterPWMInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PWMSim::GetInitialized() const {
  return HALSIM_GetPWMInitialized(m_index);
}

void PWMSim::SetInitialized(bool initialized) {
  HALSIM_SetPWMInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> PWMSim::RegisterPulseMicrosecondCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMPulseMicrosecondCallback);
  store->SetUid(HALSIM_RegisterPWMPulseMicrosecondCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int32_t PWMSim::GetPulseMicrosecond() const {
  return HALSIM_GetPWMPulseMicrosecond(m_index);
}

void PWMSim::SetPulseMicrosecond(int32_t microsecondPulseTime) {
  HALSIM_SetPWMPulseMicrosecond(m_index, microsecondPulseTime);
}

std::unique_ptr<CallbackStore> PWMSim::RegisterOutputPeriodCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMOutputPeriodCallback);
  store->SetUid(HALSIM_RegisterPWMOutputPeriodCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int PWMSim::GetOutputPeriod() const {
  return HALSIM_GetPWMOutputPeriod(m_index);
}

void PWMSim::SetOutputPeriod(int period) {
  HALSIM_SetPWMOutputPeriod(m_index, period);
}

void PWMSim::ResetData() {
  HALSIM_ResetPWMData(m_index);
}
