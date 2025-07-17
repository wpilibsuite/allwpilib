// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleSim.h"

#include <memory>
#include <stdexcept>

#include <hal/simulation/DutyCycleData.h>

#include "frc/DutyCycle.h"

using namespace frc;
using namespace frc::sim;

DutyCycleSim::DutyCycleSim(const DutyCycle& dutyCycle)
    : m_index{dutyCycle.GetSourceChannel()} {}

DutyCycleSim DutyCycleSim::CreateForChannel(int channel) {
  return DutyCycleSim{channel};
}

std::unique_ptr<CallbackStore> DutyCycleSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDutyCycleInitializedCallback);
  store->SetUid(HALSIM_RegisterDutyCycleInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DutyCycleSim::GetInitialized() const {
  return HALSIM_GetDutyCycleInitialized(m_index);
}

void DutyCycleSim::SetInitialized(bool initialized) {
  HALSIM_SetDutyCycleInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> DutyCycleSim::RegisterFrequencyCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDutyCycleFrequencyCallback);
  store->SetUid(HALSIM_RegisterDutyCycleFrequencyCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

units::hertz_t DutyCycleSim::GetFrequency() const {
  return units::hertz_t{HALSIM_GetDutyCycleFrequency(m_index)};
}

void DutyCycleSim::SetFrequency(units::hertz_t frequency) {
  HALSIM_SetDutyCycleFrequency(m_index, frequency.value());
}

std::unique_ptr<CallbackStore> DutyCycleSim::RegisterOutputCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDutyCycleOutputCallback);
  store->SetUid(HALSIM_RegisterDutyCycleOutputCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double DutyCycleSim::GetOutput() const {
  return HALSIM_GetDutyCycleOutput(m_index);
}

void DutyCycleSim::SetOutput(double output) {
  HALSIM_SetDutyCycleOutput(m_index, output);
}

void DutyCycleSim::ResetData() {
  HALSIM_ResetDutyCycleData(m_index);
}
