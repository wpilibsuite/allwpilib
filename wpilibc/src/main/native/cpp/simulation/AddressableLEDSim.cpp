// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AddressableLEDSim.hpp"

#include <memory>
#include <stdexcept>

#include "wpi/hal/simulation/AddressableLEDData.h"
#include "wpi/hardware/led/AddressableLED.hpp"

using namespace wpi;
using namespace wpi::sim;

AddressableLEDSim::AddressableLEDSim(int channel) : m_channel{channel} {}

AddressableLEDSim::AddressableLEDSim(const AddressableLED& addressableLED)
    : m_channel{addressableLED.GetChannel()} {}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_channel, -1, callback, &HALSIM_CancelAddressableLEDInitializedCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDInitializedCallback(
      m_channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AddressableLEDSim::GetInitialized() const {
  return HALSIM_GetAddressableLEDInitialized(m_channel);
}

void AddressableLEDSim::SetInitialized(bool initialized) {
  HALSIM_SetAddressableLEDInitialized(m_channel, initialized);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterStartCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_channel, -1, callback, &HALSIM_CancelAddressableLEDStartCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDStartCallback(
      m_channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AddressableLEDSim::GetStart() const {
  return HALSIM_GetAddressableLEDStart(m_channel);
}

void AddressableLEDSim::SetStart(int start) {
  HALSIM_SetAddressableLEDStart(m_channel, start);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterLengthCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_channel, -1, callback, &HALSIM_CancelAddressableLEDLengthCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDLengthCallback(
      m_channel, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AddressableLEDSim::GetLength() const {
  return HALSIM_GetAddressableLEDLength(m_channel);
}

void AddressableLEDSim::SetLength(int length) {
  HALSIM_SetAddressableLEDLength(m_channel, length);
}

int AddressableLEDSim::GetData(struct HAL_AddressableLEDData* data) const {
  return GetGlobalData(GetStart(), GetLength(), data);
}

void AddressableLEDSim::SetData(struct HAL_AddressableLEDData* data) {
  SetGlobalData(GetStart(), GetLength(), data);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterDataCallback(
    ConstBufferCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      -1, callback, &HALSIM_CancelAddressableLEDDataCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDDataCallback(
      &ConstBufferCallbackStoreThunk, store.get()));
  return store;
}

int AddressableLEDSim::GetGlobalData(int start, int length,
                                     struct HAL_AddressableLEDData* data) {
  return HALSIM_GetAddressableLEDData(start, length, data);
}

void AddressableLEDSim::SetGlobalData(int start, int length,
                                      struct HAL_AddressableLEDData* data) {
  HALSIM_SetAddressableLEDData(start, length, data);
}
