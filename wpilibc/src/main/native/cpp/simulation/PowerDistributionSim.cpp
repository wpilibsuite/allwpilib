// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PowerDistributionSim.h"

#include <memory>

#include <hal/simulation/PowerDistributionData.h>

#include "frc/PowerDistribution.h"

using namespace frc;
using namespace frc::sim;

PowerDistributionSim::PowerDistributionSim(int module) : m_index{module} {}

PowerDistributionSim::PowerDistributionSim(const PowerDistribution& pdp)
    : m_index{pdp.GetModule()} {}

std::unique_ptr<CallbackStore>
PowerDistributionSim::RegisterInitializedCallback(NotifyCallback callback,
                                                  bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback,
      &HALSIM_CancelPowerDistributionInitializedCallback);
  store->SetUid(HALSIM_RegisterPowerDistributionInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PowerDistributionSim::GetInitialized() const {
  return HALSIM_GetPowerDistributionInitialized(m_index);
}

void PowerDistributionSim::SetInitialized(bool initialized) {
  HALSIM_SetPowerDistributionInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore>
PowerDistributionSim::RegisterTemperatureCallback(NotifyCallback callback,
                                                  bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback,
      &HALSIM_CancelPowerDistributionTemperatureCallback);
  store->SetUid(HALSIM_RegisterPowerDistributionTemperatureCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PowerDistributionSim::GetTemperature() const {
  return HALSIM_GetPowerDistributionTemperature(m_index);
}

void PowerDistributionSim::SetTemperature(double temperature) {
  HALSIM_SetPowerDistributionTemperature(m_index, temperature);
}

std::unique_ptr<CallbackStore> PowerDistributionSim::RegisterVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPowerDistributionVoltageCallback);
  store->SetUid(HALSIM_RegisterPowerDistributionVoltageCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PowerDistributionSim::GetVoltage() const {
  return HALSIM_GetPowerDistributionVoltage(m_index);
}

void PowerDistributionSim::SetVoltage(double voltage) {
  HALSIM_SetPowerDistributionVoltage(m_index, voltage);
}

std::unique_ptr<CallbackStore> PowerDistributionSim::RegisterCurrentCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback,
      &HALSIM_CancelPowerDistributionCurrentCallback);
  store->SetUid(HALSIM_RegisterPowerDistributionCurrentCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PowerDistributionSim::GetCurrent(int channel) const {
  return HALSIM_GetPowerDistributionCurrent(m_index, channel);
}

void PowerDistributionSim::SetCurrent(int channel, double current) {
  HALSIM_SetPowerDistributionCurrent(m_index, channel, current);
}

void PowerDistributionSim::GetAllCurrents(double* currents, int length) const {
  HALSIM_GetPowerDistributionAllCurrents(m_index, currents, length);
}

void PowerDistributionSim::SetAllCurrents(const double* currents, int length) {
  HALSIM_SetPowerDistributionAllCurrents(m_index, currents, length);
}

void PowerDistributionSim::ResetData() {
  HALSIM_ResetPowerDistributionData(m_index);
}
