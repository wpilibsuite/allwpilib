// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogTriggerSim.h"

#include <memory>
#include <stdexcept>

#include <hal/simulation/AnalogTriggerData.h>

#include "frc/AnalogTrigger.h"

using namespace frc;
using namespace frc::sim;

AnalogTriggerSim::AnalogTriggerSim(const AnalogTrigger& analogTrigger)
    : m_index{analogTrigger.GetIndex()} {}

AnalogTriggerSim AnalogTriggerSim::CreateForChannel(int channel) {
  int index = HALSIM_FindAnalogTriggerForChannel(channel);
  if (index < 0) {
    throw std::out_of_range("no analog trigger found for channel");
  }
  return AnalogTriggerSim{index};
}

AnalogTriggerSim AnalogTriggerSim::CreateForIndex(int index) {
  return AnalogTriggerSim{index};
}

std::unique_ptr<CallbackStore> AnalogTriggerSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogTriggerInitializedCallback);
  store->SetUid(HALSIM_RegisterAnalogTriggerInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AnalogTriggerSim::GetInitialized() const {
  return HALSIM_GetAnalogTriggerInitialized(m_index);
}

void AnalogTriggerSim::SetInitialized(bool initialized) {
  HALSIM_SetAnalogTriggerInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore>
AnalogTriggerSim::RegisterTriggerLowerBoundCallback(NotifyCallback callback,
                                                    bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback,
      &HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback);
  store->SetUid(HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogTriggerSim::GetTriggerLowerBound() const {
  return HALSIM_GetAnalogTriggerTriggerLowerBound(m_index);
}

void AnalogTriggerSim::SetTriggerLowerBound(double triggerLowerBound) {
  HALSIM_SetAnalogTriggerTriggerLowerBound(m_index, triggerLowerBound);
}

std::unique_ptr<CallbackStore>
AnalogTriggerSim::RegisterTriggerUpperBoundCallback(NotifyCallback callback,
                                                    bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback,
      &HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback);
  store->SetUid(HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogTriggerSim::GetTriggerUpperBound() const {
  return HALSIM_GetAnalogTriggerTriggerUpperBound(m_index);
}

void AnalogTriggerSim::SetTriggerUpperBound(double triggerUpperBound) {
  HALSIM_SetAnalogTriggerTriggerUpperBound(m_index, triggerUpperBound);
}

void AnalogTriggerSim::ResetData() {
  HALSIM_ResetAnalogTriggerData(m_index);
}
