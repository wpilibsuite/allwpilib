// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogOutputSim.h"

#include <memory>

#include <hal/simulation/AnalogOutData.h>

#include "frc/AnalogOutput.h"

using namespace frc;
using namespace frc::sim;

AnalogOutputSim::AnalogOutputSim(const AnalogOutput& analogOutput)
    : m_index{analogOutput.GetChannel()} {}

AnalogOutputSim::AnalogOutputSim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> AnalogOutputSim::RegisterVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogOutVoltageCallback);
  store->SetUid(HALSIM_RegisterAnalogOutVoltageCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogOutputSim::GetVoltage() const {
  return HALSIM_GetAnalogOutVoltage(m_index);
}

void AnalogOutputSim::SetVoltage(double voltage) {
  HALSIM_SetAnalogOutVoltage(m_index, voltage);
}

std::unique_ptr<CallbackStore> AnalogOutputSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogOutInitializedCallback);
  store->SetUid(HALSIM_RegisterAnalogOutInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AnalogOutputSim::GetInitialized() const {
  return HALSIM_GetAnalogOutInitialized(m_index);
}

void AnalogOutputSim::SetInitialized(bool initialized) {
  HALSIM_SetAnalogOutInitialized(m_index, initialized);
}

void AnalogOutputSim::ResetData() {
  HALSIM_ResetAnalogOutData(m_index);
}
