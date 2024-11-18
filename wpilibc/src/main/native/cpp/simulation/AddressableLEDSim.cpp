// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AddressableLEDSim.h"

#include <memory>
#include <stdexcept>

#include <hal/simulation/AddressableLEDData.h>

using namespace frc;
using namespace frc::sim;

AddressableLEDSim::AddressableLEDSim() : m_index{0} {}

AddressableLEDSim::AddressableLEDSim(const AddressableLED& addressableLED)
    : m_index{0} {}

AddressableLEDSim AddressableLEDSim::CreateForChannel(int pwmChannel) {
  int index = HALSIM_FindAddressableLEDForChannel(pwmChannel);
  if (index < 0) {
    throw std::out_of_range("no addressable LED found for PWM channel");
  }
  return AddressableLEDSim{index};
}

AddressableLEDSim AddressableLEDSim::CreateForIndex(int index) {
  return AddressableLEDSim{index};
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAddressableLEDInitializedCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool AddressableLEDSim::GetInitialized() const {
  return HALSIM_GetAddressableLEDInitialized(m_index);
}

void AddressableLEDSim::SetInitialized(bool initialized) {
  HALSIM_SetAddressableLEDInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterOutputPortCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAddressableLEDOutputPortCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDOutputPortCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AddressableLEDSim::GetOutputPort() const {
  return HALSIM_GetAddressableLEDOutputPort(m_index);
}

void AddressableLEDSim::SetOutputPort(int outputPort) {
  HALSIM_SetAddressableLEDOutputPort(m_index, outputPort);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterLengthCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAddressableLEDLengthCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDLengthCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AddressableLEDSim::GetLength() const {
  return HALSIM_GetAddressableLEDLength(m_index);
}

void AddressableLEDSim::SetLength(int length) {
  HALSIM_SetAddressableLEDLength(m_index, length);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterRunningCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAddressableLEDRunningCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDRunningCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int AddressableLEDSim::GetRunning() const {
  return HALSIM_GetAddressableLEDRunning(m_index);
}

void AddressableLEDSim::SetRunning(bool running) {
  HALSIM_SetAddressableLEDRunning(m_index, running);
}

std::unique_ptr<CallbackStore> AddressableLEDSim::RegisterDataCallback(
    ConstBufferCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelAddressableLEDDataCallback);
  store->SetUid(HALSIM_RegisterAddressableLEDDataCallback(
      m_index, &ConstBufferCallbackStoreThunk, store.get()));
  return store;
}

int AddressableLEDSim::GetData(struct HAL_AddressableLEDData* data) const {
  return HALSIM_GetAddressableLEDData(m_index, data);
}

void AddressableLEDSim::SetData(struct HAL_AddressableLEDData* data,
                                int length) {
  HALSIM_SetAddressableLEDData(m_index, data, length);
}
