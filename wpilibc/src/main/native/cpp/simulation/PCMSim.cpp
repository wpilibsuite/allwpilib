/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/PCMSim.h"

#include <memory>
#include <utility>

#include <hal/simulation/PCMData.h>

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
      &HALSIM_CancelPCMSolenoidInitializedCallback);
  store->SetUid(HALSIM_RegisterPCMSolenoidInitializedCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetSolenoidInitialized(int channel) const {
  return HALSIM_GetPCMSolenoidInitialized(m_index, channel);
}

void PCMSim::SetSolenoidInitialized(int channel, bool solenoidInitialized) {
  HALSIM_SetPCMSolenoidInitialized(m_index, channel, solenoidInitialized);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterSolenoidOutputCallback(
    int channel, NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, channel, -1, callback, &HALSIM_CancelPCMSolenoidOutputCallback);
  store->SetUid(HALSIM_RegisterPCMSolenoidOutputCallback(
      m_index, channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetSolenoidOutput(int channel) const {
  return HALSIM_GetPCMSolenoidOutput(m_index, channel);
}

void PCMSim::SetSolenoidOutput(int channel, bool solenoidOutput) {
  HALSIM_SetPCMSolenoidOutput(m_index, channel, solenoidOutput);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPCMCompressorInitializedCallback);
  store->SetUid(HALSIM_RegisterPCMCompressorInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetCompressorInitialized() const {
  return HALSIM_GetPCMCompressorInitialized(m_index);
}

void PCMSim::SetCompressorInitialized(bool compressorInitialized) {
  HALSIM_SetPCMCompressorInitialized(m_index, compressorInitialized);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorOnCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPCMCompressorOnCallback);
  store->SetUid(HALSIM_RegisterPCMCompressorOnCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetCompressorOn() const {
  return HALSIM_GetPCMCompressorOn(m_index);
}

void PCMSim::SetCompressorOn(bool compressorOn) {
  HALSIM_SetPCMCompressorOn(m_index, compressorOn);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterClosedLoopEnabledCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPCMClosedLoopEnabledCallback);
  store->SetUid(HALSIM_RegisterPCMClosedLoopEnabledCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetClosedLoopEnabled() const {
  return HALSIM_GetPCMClosedLoopEnabled(m_index);
}

void PCMSim::SetClosedLoopEnabled(bool closedLoopEnabled) {
  HALSIM_SetPCMClosedLoopEnabled(m_index, closedLoopEnabled);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterPressureSwitchCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPCMPressureSwitchCallback);
  store->SetUid(HALSIM_RegisterPCMPressureSwitchCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool PCMSim::GetPressureSwitch() const {
  return HALSIM_GetPCMPressureSwitch(m_index);
}

void PCMSim::SetPressureSwitch(bool pressureSwitch) {
  HALSIM_SetPCMPressureSwitch(m_index, pressureSwitch);
}

std::unique_ptr<CallbackStore> PCMSim::RegisterCompressorCurrentCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelPCMCompressorCurrentCallback);
  store->SetUid(HALSIM_RegisterPCMCompressorCurrentCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double PCMSim::GetCompressorCurrent() const {
  return HALSIM_GetPCMCompressorCurrent(m_index);
}

void PCMSim::SetCompressorCurrent(double compressorCurrent) {
  HALSIM_SetPCMCompressorCurrent(m_index, compressorCurrent);
}

uint8_t PCMSim::GetAllSolenoidOutputs() const {
  uint8_t ret = 0;
  HALSIM_GetPCMAllSolenoids(m_index, &ret);
  return ret;
}

void PCMSim::SetAllSolenoidOutputs(uint8_t outputs) {
  HALSIM_SetPCMAllSolenoids(m_index, outputs);
}

void PCMSim::ResetData() { HALSIM_ResetPCMData(m_index); }
