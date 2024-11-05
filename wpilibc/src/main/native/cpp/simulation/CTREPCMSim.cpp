// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/CTREPCMSim.h"

#include <memory>

#include <hal/simulation/CTREPCMData.h>

#include "frc/SensorUtil.h"

using namespace frc;
using namespace frc::sim;

CTREPCMSim::CTREPCMSim()
    : PneumaticsBaseSim{SensorUtil::GetDefaultCTREPCMModule()} {}

CTREPCMSim::CTREPCMSim(int module) : PneumaticsBaseSim{module} {}

CTREPCMSim::CTREPCMSim(const PneumaticsBase& pneumatics)
    : PneumaticsBaseSim{pneumatics.GetModuleNumber()} {}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMInitializedCallback);
  store->SetUid(HALSIM_RegisterCTREPCMInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool CTREPCMSim::GetInitialized() const {
  return HALSIM_GetCTREPCMInitialized(m_index);
}

void CTREPCMSim::SetInitialized(bool solenoidInitialized) {
  HALSIM_SetCTREPCMInitialized(m_index, solenoidInitialized);
}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterSolenoidOutputCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback,
      &HALSIM_CancelCTREPCMSolenoidOutputCallback);
  store->SetUid(HALSIM_RegisterCTREPCMSolenoidOutputCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool CTREPCMSim::GetSolenoidOutput(int channel) const {
  return HALSIM_GetCTREPCMSolenoidOutput(m_index, channel);
}

void CTREPCMSim::SetSolenoidOutput(int channel, bool solenoidOutput) {
  HALSIM_SetCTREPCMSolenoidOutput(m_index, channel, solenoidOutput);
}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterCompressorOnCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMCompressorOnCallback);
  store->SetUid(HALSIM_RegisterCTREPCMCompressorOnCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool CTREPCMSim::GetCompressorOn() const {
  return HALSIM_GetCTREPCMCompressorOn(m_index);
}

void CTREPCMSim::SetCompressorOn(bool compressorOn) {
  HALSIM_SetCTREPCMCompressorOn(m_index, compressorOn);
}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterClosedLoopEnabledCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMClosedLoopEnabledCallback);
  store->SetUid(HALSIM_RegisterCTREPCMClosedLoopEnabledCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool CTREPCMSim::GetClosedLoopEnabled() const {
  return HALSIM_GetCTREPCMClosedLoopEnabled(m_index);
}

void CTREPCMSim::SetClosedLoopEnabled(bool closedLoopEnabled) {
  HALSIM_SetCTREPCMClosedLoopEnabled(m_index, closedLoopEnabled);
}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterPressureSwitchCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMPressureSwitchCallback);
  store->SetUid(HALSIM_RegisterCTREPCMPressureSwitchCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool CTREPCMSim::GetPressureSwitch() const {
  return HALSIM_GetCTREPCMPressureSwitch(m_index);
}

void CTREPCMSim::SetPressureSwitch(bool pressureSwitch) {
  HALSIM_SetCTREPCMPressureSwitch(m_index, pressureSwitch);
}

std::unique_ptr<CallbackStore> CTREPCMSim::RegisterCompressorCurrentCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMCompressorCurrentCallback);
  store->SetUid(HALSIM_RegisterCTREPCMCompressorCurrentCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double CTREPCMSim::GetCompressorCurrent() const {
  return HALSIM_GetCTREPCMCompressorCurrent(m_index);
}

void CTREPCMSim::SetCompressorCurrent(double compressorCurrent) {
  HALSIM_SetCTREPCMCompressorCurrent(m_index, compressorCurrent);
}

uint8_t CTREPCMSim::GetAllSolenoidOutputs() const {
  uint8_t ret = 0;
  HALSIM_GetCTREPCMAllSolenoids(m_index, &ret);
  return ret;
}

void CTREPCMSim::SetAllSolenoidOutputs(uint8_t outputs) {
  HALSIM_SetCTREPCMAllSolenoids(m_index, outputs);
}

void CTREPCMSim::ResetData() {
  HALSIM_ResetCTREPCMData(m_index);
}
