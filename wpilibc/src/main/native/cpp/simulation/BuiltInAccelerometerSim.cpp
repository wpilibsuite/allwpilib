// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/BuiltInAccelerometerSim.h"

#include <memory>

#include <hal/simulation/AccelerometerData.h>

#include "frc/BuiltInAccelerometer.h"

using namespace frc;
using namespace frc::sim;

BuiltInAccelerometerSim::BuiltInAccelerometerSim() : m_index{0} {}

BuiltInAccelerometerSim::BuiltInAccelerometerSim(const BuiltInAccelerometer&)
    : m_index{0} {}

std::unique_ptr<CallbackStore> BuiltInAccelerometerSim::RegisterActiveCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAccelerometerActiveCallback);
  store->SetUid(HALSIM_RegisterAccelerometerActiveCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool BuiltInAccelerometerSim::GetActive() const {
  return HALSIM_GetAccelerometerActive(m_index);
}

void BuiltInAccelerometerSim::SetActive(bool active) {
  HALSIM_SetAccelerometerActive(m_index, active);
}

std::unique_ptr<CallbackStore> BuiltInAccelerometerSim::RegisterRangeCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAccelerometerRangeCallback);
  store->SetUid(HALSIM_RegisterAccelerometerRangeCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

HAL_AccelerometerRange BuiltInAccelerometerSim::GetRange() const {
  return HALSIM_GetAccelerometerRange(m_index);
}

void BuiltInAccelerometerSim::SetRange(HAL_AccelerometerRange range) {
  HALSIM_SetAccelerometerRange(m_index, range);
}

std::unique_ptr<CallbackStore> BuiltInAccelerometerSim::RegisterXCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAccelerometerXCallback);
  store->SetUid(HALSIM_RegisterAccelerometerXCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double BuiltInAccelerometerSim::GetX() const {
  return HALSIM_GetAccelerometerX(m_index);
}

void BuiltInAccelerometerSim::SetX(double x) {
  HALSIM_SetAccelerometerX(m_index, x);
}

std::unique_ptr<CallbackStore> BuiltInAccelerometerSim::RegisterYCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAccelerometerYCallback);
  store->SetUid(HALSIM_RegisterAccelerometerYCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double BuiltInAccelerometerSim::GetY() const {
  return HALSIM_GetAccelerometerY(m_index);
}

void BuiltInAccelerometerSim::SetY(double y) {
  HALSIM_SetAccelerometerY(m_index, y);
}

std::unique_ptr<CallbackStore> BuiltInAccelerometerSim::RegisterZCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAccelerometerZCallback);
  store->SetUid(HALSIM_RegisterAccelerometerZCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double BuiltInAccelerometerSim::GetZ() const {
  return HALSIM_GetAccelerometerZ(m_index);
}

void BuiltInAccelerometerSim::SetZ(double z) {
  HALSIM_SetAccelerometerZ(m_index, z);
}

void BuiltInAccelerometerSim::ResetData() {
  HALSIM_ResetAccelerometerData(m_index);
}
