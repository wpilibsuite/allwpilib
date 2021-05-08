// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PCMSim.h"

#include <memory>
#include <utility>

#include <hal/simulation/CTREPCMData.h>

#include "frc/Compressor.h"
#include "frc/SensorUtil.h"

using namespace frc;
using namespace frc::sim;

PCMSim::PCMSim() : m_index{SensorUtil::GetDefaultSolenoidModule()} {}

PCMSim::PCMSim(int module) : m_index{module} {}

PCMSim::PCMSim(const Compressor& compressor)
    : m_index{compressor.GetModule()} {}

std::unique_ptr<CallbackStore> PCMSim::RegisterSolenoidInitializedCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback,
      &HALSIM_CancelCTREPCMSolenoidInitializedCallback);
  store->SetUid(HALSIM_RegisterCTREPCMSolenoidInitializedCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetSolenoidInitialized(int channel) const {
  return HALSIM_GetCTREPCMSolenoidInitialized(m_index, channel);
}

void PCMSim::SetSolenoidInitialized(int channel, bool solenoidInitialized) {
  HALSIM_SetCTREPCMSolenoidInitialized(m_index, channel, solenoidInitialized);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterSolenoidOutputCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback, &HALSIM_CancelCTREPCMSolenoidOutputCallback);
  store->SetUid(HALSIM_RegisterCTREPCMSolenoidOutputCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetSolenoidOutput(int channel) const {
  return HALSIM_GetCTREPCMSolenoidOutput(m_index, channel);
}

void PCMSim::SetSolenoidOutput(int channel, bool solenoidOutput) {
  HALSIM_SetCTREPCMSolenoidOutput(m_index, channel, solenoidOutput);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMCompressorInitializedCallback);
  store->SetUid(HALSIM_RegisterCTREPCMCompressorInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetCompressorInitialized() const {
  return HALSIM_GetCTREPCMCompressorInitialized(m_index);
}

void PCMSim::SetCompressorInitialized(bool compressorInitialized) {
  HALSIM_SetCTREPCMCompressorInitialized(m_index, compressorInitialized);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorOnCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMCompressorOnCallback);
  store->SetUid(HALSIM_RegisterCTREPCMCompressorOnCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetCompressorOn() const {
  return HALSIM_GetCTREPCMCompressorOn(m_index);
}

void PCMSim::SetCompressorOn(bool compressorOn) {
  HALSIM_SetCTREPCMCompressorOn(m_index, compressorOn);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterClosedLoopEnabledCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMClosedLoopEnabledCallback);
  store->SetUid(HALSIM_RegisterCTREPCMClosedLoopEnabledCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetClosedLoopEnabled() const {
  return HALSIM_GetCTREPCMClosedLoopEnabled(m_index);
}

void PCMSim::SetClosedLoopEnabled(bool closedLoopEnabled) {
  HALSIM_SetCTREPCMClosedLoopEnabled(m_index, closedLoopEnabled);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterPressureSwitchCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMPressureSwitchCallback);
  store->SetUid(HALSIM_RegisterCTREPCMPressureSwitchCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetPressureSwitch() const {
  return HALSIM_GetCTREPCMPressureSwitch(m_index);
}

void PCMSim::SetPressureSwitch(bool pressureSwitch) {
  HALSIM_SetCTREPCMPressureSwitch(m_index, pressureSwitch);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorCurrentCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelCTREPCMCompressorCurrentCallback);
  store->SetUid(HALSIM_RegisterCTREPCMCompressorCurrentCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PCMSim::GetCompressorCurrent() const {
  return HALSIM_GetCTREPCMCompressorCurrent(m_index);
}

void PCMSim::SetCompressorCurrent(double compressorCurrent) {
  HALSIM_SetCTREPCMCompressorCurrent(m_index, compressorCurrent);
}

uint8_t PCMSim::GetAllSolenoidOutputs() const {
  uint8_t ret = 0;
  HALSIM_GetCTREPCMAllSolenoids(m_index, &ret);
  return ret;
}

void PCMSim::SetAllSolenoidOutputs(uint8_t outputs) {
  HALSIM_SetCTREPCMAllSolenoids(m_index, outputs);
}

void PCMSim::ResetData() {
  HALSIM_ResetCTREPCMData(m_index);
}
