// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogGyroSim.h"

#include <memory>

#include <hal/simulation/AnalogGyroData.h>

#include "frc/AnalogGyro.h"
#include "frc/AnalogInput.h"

using namespace frc;
using namespace frc::sim;

AnalogGyroSim::AnalogGyroSim(const AnalogGyro& gyro)
    : m_index{gyro.GetAnalogInput()->GetChannel()} {}

AnalogGyroSim::AnalogGyroSim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> AnalogGyroSim::RegisterAngleCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogGyroAngleCallback);
  store->SetUid(HALSIM_RegisterAnalogGyroAngleCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogGyroSim::GetAngle() const {
  return HALSIM_GetAnalogGyroAngle(m_index);
}

void AnalogGyroSim::SetAngle(double angle) {
  HALSIM_SetAnalogGyroAngle(m_index, angle);
}

std::unique_ptr<CallbackStore> AnalogGyroSim::RegisterRateCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogGyroRateCallback);
  store->SetUid(HALSIM_RegisterAnalogGyroRateCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double AnalogGyroSim::GetRate() const {
  return HALSIM_GetAnalogGyroRate(m_index);
}

void AnalogGyroSim::SetRate(double rate) {
  HALSIM_SetAnalogGyroRate(m_index, rate);
}

std::unique_ptr<CallbackStore> AnalogGyroSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAnalogGyroInitializedCallback);
  store->SetUid(HALSIM_RegisterAnalogGyroInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AnalogGyroSim::GetInitialized() const {
  return HALSIM_GetAnalogGyroInitialized(m_index);
}

void AnalogGyroSim::SetInitialized(bool initialized) {
  HALSIM_SetAnalogGyroInitialized(m_index, initialized);
}

void AnalogGyroSim::ResetData() {
  HALSIM_ResetAnalogGyroData(m_index);
}
