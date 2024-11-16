// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMSim.h"

#include <memory>

#include <hal/simulation/PWMData.h>

#include "frc/PWM.h"
#include "frc/motorcontrol/PWMMotorController.h"

using namespace frc;
using namespace frc::sim;

PWMSim::PWMSim(const PWM& pwm) : m_index{pwm.GetChannel()} {}

PWMSim::PWMSim(const PWMMotorController& motorctrl)
    : m_index{motorctrl.GetChannel()} {}

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

std::unique_ptr<CallbackStore> PWMSim::RegisterSpeedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(m_index, -1, callback,
                                               &HALSIM_CancelPWMSpeedCallback);
  store->SetUid(HALSIM_RegisterPWMSpeedCallback(m_index, &CallbackStoreThunk,
                                                store.get(), initialNotify));
  return store;
}

double PWMSim::GetSpeed() const {
  return HALSIM_GetPWMSpeed(m_index);
}

void PWMSim::SetSpeed(double speed) {
  HALSIM_SetPWMSpeed(m_index, speed);
}

std::unique_ptr<CallbackStore> PWMSim::RegisterPositionCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMPositionCallback);
  store->SetUid(HALSIM_RegisterPWMPositionCallback(m_index, &CallbackStoreThunk,
                                                   store.get(), initialNotify));
  return store;
}

double PWMSim::GetPosition() const {
  return HALSIM_GetPWMPosition(m_index);
}

void PWMSim::SetPosition(double position) {
  HALSIM_SetPWMPosition(m_index, position);
}

std::unique_ptr<CallbackStore> PWMSim::RegisterPeriodScaleCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMPeriodScaleCallback);
  store->SetUid(HALSIM_RegisterPWMPeriodScaleCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int PWMSim::GetPeriodScale() const {
  return HALSIM_GetPWMPeriodScale(m_index);
}

void PWMSim::SetPeriodScale(int periodScale) {
  HALSIM_SetPWMPeriodScale(m_index, periodScale);
}

std::unique_ptr<CallbackStore> PWMSim::RegisterZeroLatchCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPWMZeroLatchCallback);
  store->SetUid(HALSIM_RegisterPWMZeroLatchCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PWMSim::GetZeroLatch() const {
  return HALSIM_GetPWMZeroLatch(m_index);
}

void PWMSim::SetZeroLatch(bool zeroLatch) {
  HALSIM_SetPWMZeroLatch(m_index, zeroLatch);
}

void PWMSim::ResetData() {
  HALSIM_ResetPWMData(m_index);
}
