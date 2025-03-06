// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogInputSim.h"

#include <memory>

#include <hal/simulation/AnalogInData.h>

#include "frc/AnalogInput.h"

using namespace frc;
using namespace frc::sim;

AnalogInputSim::AnalogInputSim(const AnalogInput& analogInput)
    : m_index{analogInput.GetChannel()} {}

AnalogInputSim::AnalogInputSim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> AnalogInputSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogInInitializedCallback);
  store->SetUid(HALSIM_RegisterAnalogInInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AnalogInputSim::GetInitialized() const {
  return HALSIM_GetAnalogInInitialized(m_index);
}

void AnalogInputSim::SetInitialized(bool initialized) {
  HALSIM_SetAnalogInInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> AnalogInputSim::RegisterVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogInVoltageCallback);
  store->SetUid(HALSIM_RegisterAnalogInVoltageCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogInputSim::GetVoltage() const {
  return HALSIM_GetAnalogInVoltage(m_index);
}

void AnalogInputSim::SetVoltage(double voltage) {
  HALSIM_SetAnalogInVoltage(m_index, voltage);
}

void AnalogInputSim::ResetData() {
  HALSIM_ResetAnalogInData(m_index);
}
