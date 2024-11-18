// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DIOSim.h"

#include <memory>

#include <hal/simulation/DIOData.h>

#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"

using namespace frc;
using namespace frc::sim;

DIOSim::DIOSim(const DigitalInput& input) : m_index{input.GetChannel()} {}

DIOSim::DIOSim(const DigitalOutput& output) : m_index{output.GetChannel()} {}

DIOSim::DIOSim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> DIOSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDIOInitializedCallback);
  store->SetUid(HALSIM_RegisterDIOInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DIOSim::GetInitialized() const {
  return HALSIM_GetDIOInitialized(m_index);
}

void DIOSim::SetInitialized(bool initialized) {
  HALSIM_SetDIOInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> DIOSim::RegisterValueCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(m_index, -1, callback,
                                               &HALSIM_CancelDIOValueCallback);
  store->SetUid(HALSIM_RegisterDIOValueCallback(m_index, &CallbackStoreThunk,
                                                store.get(), initialNotify));
  return store;
}

bool DIOSim::GetValue() const {
  return HALSIM_GetDIOValue(m_index);
}

void DIOSim::SetValue(bool value) {
  HALSIM_SetDIOValue(m_index, value);
}

std::unique_ptr<CallbackStore> DIOSim::RegisterPulseLengthCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDIOPulseLengthCallback);
  store->SetUid(HALSIM_RegisterDIOPulseLengthCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double DIOSim::GetPulseLength() const {
  return HALSIM_GetDIOPulseLength(m_index);
}

void DIOSim::SetPulseLength(double pulseLength) {
  HALSIM_SetDIOPulseLength(m_index, pulseLength);
}

std::unique_ptr<CallbackStore> DIOSim::RegisterIsInputCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDIOIsInputCallback);
  store->SetUid(HALSIM_RegisterDIOIsInputCallback(m_index, &CallbackStoreThunk,
                                                  store.get(), initialNotify));
  return store;
}

bool DIOSim::GetIsInput() const {
  return HALSIM_GetDIOIsInput(m_index);
}

void DIOSim::SetIsInput(bool isInput) {
  HALSIM_SetDIOIsInput(m_index, isInput);
}

std::unique_ptr<CallbackStore> DIOSim::RegisterFilterIndexCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDIOFilterIndexCallback);
  store->SetUid(HALSIM_RegisterDIOFilterIndexCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int DIOSim::GetFilterIndex() const {
  return HALSIM_GetDIOFilterIndex(m_index);
}

void DIOSim::SetFilterIndex(int filterIndex) {
  HALSIM_SetDIOFilterIndex(m_index, filterIndex);
}

void DIOSim::ResetData() {
  HALSIM_ResetDIOData(m_index);
}
