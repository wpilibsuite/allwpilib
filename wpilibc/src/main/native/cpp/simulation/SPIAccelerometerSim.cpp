// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SPIAccelerometerSim.h"

#include <memory>

#include <hal/simulation/SPIAccelerometerData.h>

using namespace frc;
using namespace frc::sim;

SPIAccelerometerSim::SPIAccelerometerSim(int index) {
  m_index = index;
}

std::unique_ptr<CallbackStore> SPIAccelerometerSim::RegisterActiveCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelSPIAccelerometerActiveCallback);
  store->SetUid(HALSIM_RegisterSPIAccelerometerActiveCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool SPIAccelerometerSim::GetActive() const {
  return HALSIM_GetSPIAccelerometerActive(m_index);
}

void SPIAccelerometerSim::SetActive(bool active) {
  HALSIM_SetSPIAccelerometerActive(m_index, active);
}

std::unique_ptr<CallbackStore> SPIAccelerometerSim::RegisterRangeCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelSPIAccelerometerRangeCallback);
  store->SetUid(HALSIM_RegisterSPIAccelerometerRangeCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int SPIAccelerometerSim::GetRange() const {
  return HALSIM_GetSPIAccelerometerRange(m_index);
}

void SPIAccelerometerSim::SetRange(int range) {
  HALSIM_SetSPIAccelerometerRange(m_index, range);
}

std::unique_ptr<CallbackStore> SPIAccelerometerSim::RegisterXCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelSPIAccelerometerXCallback);
  store->SetUid(HALSIM_RegisterSPIAccelerometerXCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double SPIAccelerometerSim::GetX() const {
  return HALSIM_GetSPIAccelerometerX(m_index);
}

void SPIAccelerometerSim::SetX(double x) {
  HALSIM_SetSPIAccelerometerX(m_index, x);
}

std::unique_ptr<CallbackStore> SPIAccelerometerSim::RegisterYCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelSPIAccelerometerYCallback);
  store->SetUid(HALSIM_RegisterSPIAccelerometerYCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double SPIAccelerometerSim::GetY() const {
  return HALSIM_GetSPIAccelerometerY(m_index);
}

void SPIAccelerometerSim::SetY(double y) {
  HALSIM_SetSPIAccelerometerY(m_index, y);
}

std::unique_ptr<CallbackStore> SPIAccelerometerSim::RegisterZCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelSPIAccelerometerZCallback);
  store->SetUid(HALSIM_RegisterSPIAccelerometerZCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double SPIAccelerometerSim::GetZ() const {
  return HALSIM_GetSPIAccelerometerZ(m_index);
}

void SPIAccelerometerSim::SetZ(double z) {
  HALSIM_SetSPIAccelerometerZ(m_index, z);
}

void SPIAccelerometerSim::ResetData() {
  HALSIM_ResetSPIAccelerometerData(m_index);
}
