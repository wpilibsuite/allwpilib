// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogInputSim.hpp"

#include <memory>

#include "wpi/hal/simulation/AnalogInData.h"

#include "wpi/hardware/discrete/AnalogInput.hpp"

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

std::unique_ptr<CallbackStore> AnalogInputSim::RegisterAverageBitsCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogInAverageBitsCallback);
  store->SetUid(HALSIM_RegisterAnalogInAverageBitsCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AnalogInputSim::GetAverageBits() const {
  return HALSIM_GetAnalogInAverageBits(m_index);
}

void AnalogInputSim::SetAverageBits(int averageBits) {
  HALSIM_SetAnalogInAverageBits(m_index, averageBits);
}

std::unique_ptr<CallbackStore> AnalogInputSim::RegisterOversampleBitsCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogInOversampleBitsCallback);
  store->SetUid(HALSIM_RegisterAnalogInOversampleBitsCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AnalogInputSim::GetOversampleBits() const {
  return HALSIM_GetAnalogInOversampleBits(m_index);
}

void AnalogInputSim::SetOversampleBits(int oversampleBits) {
  HALSIM_SetAnalogInOversampleBits(m_index, oversampleBits);
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
