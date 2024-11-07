// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DigitalPWMSim.h"

#include <memory>
#include <stdexcept>

#include <hal/simulation/DigitalPWMData.h>

#include "frc/DigitalOutput.h"

using namespace frc;
using namespace frc::sim;

DigitalPWMSim::DigitalPWMSim(const DigitalOutput& digitalOutput)
    : m_index{digitalOutput.GetChannel()} {}

DigitalPWMSim DigitalPWMSim::CreateForChannel(int channel) {
  int index = HALSIM_FindDigitalPWMForChannel(channel);
  if (index < 0) {
    throw std::out_of_range("no digital PWM found for channel");
  }
  return DigitalPWMSim{index};
}

DigitalPWMSim DigitalPWMSim::CreateForIndex(int index) {
  return DigitalPWMSim{index};
}

std::unique_ptr<CallbackStore> DigitalPWMSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDigitalPWMInitializedCallback);
  store->SetUid(HALSIM_RegisterDigitalPWMInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool DigitalPWMSim::GetInitialized() const {
  return HALSIM_GetDigitalPWMInitialized(m_index);
}

void DigitalPWMSim::SetInitialized(bool initialized) {
  HALSIM_SetDigitalPWMInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> DigitalPWMSim::RegisterDutyCycleCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDigitalPWMDutyCycleCallback);
  store->SetUid(HALSIM_RegisterDigitalPWMDutyCycleCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double DigitalPWMSim::GetDutyCycle() const {
  return HALSIM_GetDigitalPWMDutyCycle(m_index);
}

void DigitalPWMSim::SetDutyCycle(double dutyCycle) {
  HALSIM_SetDigitalPWMDutyCycle(m_index, dutyCycle);
}

std::unique_ptr<CallbackStore> DigitalPWMSim::RegisterPinCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelDigitalPWMPinCallback);
  store->SetUid(HALSIM_RegisterDigitalPWMPinCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int DigitalPWMSim::GetPin() const {
  return HALSIM_GetDigitalPWMPin(m_index);
}

void DigitalPWMSim::SetPin(int pin) {
  HALSIM_SetDigitalPWMPin(m_index, pin);
}

void DigitalPWMSim::ResetData() {
  HALSIM_ResetDigitalPWMData(m_index);
}
