// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/REVPHSim.h"

#include <memory>

#include <hal/simulation/REVPHData.h>

#include "frc/SensorUtil.h"

using namespace frc;
using namespace frc::sim;

REVPHSim::REVPHSim() : PneumaticsBaseSim{SensorUtil::GetDefaultREVPHModule()} {}

REVPHSim::REVPHSim(int module) : PneumaticsBaseSim{module} {}

REVPHSim::REVPHSim(const PneumaticsBase& pneumatics)
    : PneumaticsBaseSim{pneumatics} {}

std::unique_ptr<CallbackStore> REVPHSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelREVPHInitializedCallback);
  store->SetUid(HALSIM_RegisterREVPHInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool REVPHSim::GetInitialized() const {
  return HALSIM_GetREVPHInitialized(m_index);
}

void REVPHSim::SetInitialized(bool solenoidInitialized) {
  HALSIM_SetREVPHInitialized(m_index, solenoidInitialized);
}

std::unique_ptr<CallbackStore> REVPHSim::RegisterSolenoidOutputCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback,
      &HALSIM_CancelREVPHSolenoidOutputCallback);
  store->SetUid(HALSIM_RegisterREVPHSolenoidOutputCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool REVPHSim::GetSolenoidOutput(int channel) const {
  return HALSIM_GetREVPHSolenoidOutput(m_index, channel);
}

void REVPHSim::SetSolenoidOutput(int channel, bool solenoidOutput) {
  HALSIM_SetREVPHSolenoidOutput(m_index, channel, solenoidOutput);
}

std::unique_ptr<CallbackStore> REVPHSim::RegisterCompressorOnCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelREVPHCompressorOnCallback);
  store->SetUid(HALSIM_RegisterREVPHCompressorOnCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool REVPHSim::GetCompressorOn() const {
  return HALSIM_GetREVPHCompressorOn(m_index);
}

void REVPHSim::SetCompressorOn(bool compressorOn) {
  HALSIM_SetREVPHCompressorOn(m_index, compressorOn);
}

std::unique_ptr<CallbackStore> REVPHSim::RegisterCompressorConfigTypeCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelREVPHCompressorConfigTypeCallback);
  store->SetUid(HALSIM_RegisterREVPHCompressorConfigTypeCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int REVPHSim::GetCompressorConfigType() const {
  return HALSIM_GetREVPHCompressorConfigType(m_index);
}

void REVPHSim::SetCompressorConfigType(int compressorConfigType) {
  HALSIM_SetREVPHCompressorConfigType(
      m_index,
      static_cast<HAL_REVPHCompressorConfigType>(compressorConfigType));
}

std::unique_ptr<CallbackStore> REVPHSim::RegisterPressureSwitchCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelREVPHPressureSwitchCallback);
  store->SetUid(HALSIM_RegisterREVPHPressureSwitchCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool REVPHSim::GetPressureSwitch() const {
  return HALSIM_GetREVPHPressureSwitch(m_index);
}

void REVPHSim::SetPressureSwitch(bool pressureSwitch) {
  HALSIM_SetREVPHPressureSwitch(m_index, pressureSwitch);
}

std::unique_ptr<CallbackStore> REVPHSim::RegisterCompressorCurrentCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelREVPHCompressorCurrentCallback);
  store->SetUid(HALSIM_RegisterREVPHCompressorCurrentCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double REVPHSim::GetCompressorCurrent() const {
  return HALSIM_GetREVPHCompressorCurrent(m_index);
}

void REVPHSim::SetCompressorCurrent(double compressorCurrent) {
  HALSIM_SetREVPHCompressorCurrent(m_index, compressorCurrent);
}

uint8_t REVPHSim::GetAllSolenoidOutputs() const {
  uint8_t ret = 0;
  HALSIM_GetREVPHAllSolenoids(m_index, &ret);
  return ret;
}

void REVPHSim::SetAllSolenoidOutputs(uint8_t outputs) {
  HALSIM_SetREVPHAllSolenoids(m_index, outputs);
}

void REVPHSim::ResetData() {
  HALSIM_ResetREVPHData(m_index);
}
