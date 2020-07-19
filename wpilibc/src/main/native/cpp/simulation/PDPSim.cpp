/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/PDPSim.h"

#include <memory>
#include <utility>

#include <hal/simulation/PDPData.h>

#include "frc/PowerDistributionPanel.h"

using namespace frc;
using namespace frc::sim;

PDPSim::PDPSim(int module) : m_index{module} {}

PDPSim::PDPSim(const PowerDistributionPanel& pdp) : m_index{pdp.GetModule()} {}

std::unique_ptr<CallbackStore> PDPSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPDPInitializedCallback);
  store->SetUid(HALSIM_RegisterPDPInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PDPSim::GetInitialized() const {
  return HALSIM_GetPDPInitialized(m_index);
}

void PDPSim::SetInitialized(bool initialized) {
  HALSIM_SetPDPInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> PDPSim::RegisterTemperatureCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPDPTemperatureCallback);
  store->SetUid(HALSIM_RegisterPDPTemperatureCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PDPSim::GetTemperature() const {
  return HALSIM_GetPDPTemperature(m_index);
}

void PDPSim::SetTemperature(double temperature) {
  HALSIM_SetPDPTemperature(m_index, temperature);
}

std::unique_ptr<CallbackStore> PDPSim::RegisterVoltageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPDPVoltageCallback);
  store->SetUid(HALSIM_RegisterPDPVoltageCallback(m_index, &CallbackStoreThunk,
                                                  store.get(), initialNotify));
  return store;
}

double PDPSim::GetVoltage() const { return HALSIM_GetPDPVoltage(m_index); }

void PDPSim::SetVoltage(double voltage) {
  HALSIM_SetPDPVoltage(m_index, voltage);
}

std::unique_ptr<CallbackStore> PDPSim::RegisterCurrentCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback, &HALSIM_CancelPDPCurrentCallback);
  store->SetUid(HALSIM_RegisterPDPCurrentCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PDPSim::GetCurrent(int channel) const {
  return HALSIM_GetPDPCurrent(m_index, channel);
}

void PDPSim::SetCurrent(int channel, double current) {
  HALSIM_SetPDPCurrent(m_index, channel, current);
}

void PDPSim::GetAllCurrents(double* currents) const {
  HALSIM_GetPDPAllCurrents(m_index, currents);
}

void PDPSim::SetAllCurrents(const double* currents) {
  HALSIM_SetPDPAllCurrents(m_index, currents);
}

void PDPSim::ResetData() { HALSIM_ResetPDPData(m_index); }
