// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/RelaySim.h"

#include <memory>

#include <hal/simulation/RelayData.h>

#include "frc/Relay.h"

using namespace frc;
using namespace frc::sim;

RelaySim::RelaySim(const Relay& relay) : m_index{relay.GetChannel()} {}

RelaySim::RelaySim(int channel) : m_index{channel} {}

std::unique_ptr<CallbackStore> RelaySim::RegisterInitializedForwardCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelRelayInitializedForwardCallback);
  store->SetUid(HALSIM_RegisterRelayInitializedForwardCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RelaySim::GetInitializedForward() const {
  return HALSIM_GetRelayInitializedForward(m_index);
}

void RelaySim::SetInitializedForward(bool initializedForward) {
  HALSIM_SetRelayInitializedForward(m_index, initializedForward);
}

std::unique_ptr<CallbackStore> RelaySim::RegisterInitializedReverseCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelRelayInitializedReverseCallback);
  store->SetUid(HALSIM_RegisterRelayInitializedReverseCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RelaySim::GetInitializedReverse() const {
  return HALSIM_GetRelayInitializedReverse(m_index);
}

void RelaySim::SetInitializedReverse(bool initializedReverse) {
  HALSIM_SetRelayInitializedReverse(m_index, initializedReverse);
}

std::unique_ptr<CallbackStore> RelaySim::RegisterForwardCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelRelayForwardCallback);
  store->SetUid(HALSIM_RegisterRelayForwardCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RelaySim::GetForward() const {
  return HALSIM_GetRelayForward(m_index);
}

void RelaySim::SetForward(bool forward) {
  HALSIM_SetRelayForward(m_index, forward);
}

std::unique_ptr<CallbackStore> RelaySim::RegisterReverseCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelRelayReverseCallback);
  store->SetUid(HALSIM_RegisterRelayReverseCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool RelaySim::GetReverse() const {
  return HALSIM_GetRelayReverse(m_index);
}

void RelaySim::SetReverse(bool reverse) {
  HALSIM_SetRelayReverse(m_index, reverse);
}

void RelaySim::ResetData() {
  HALSIM_ResetRelayData(m_index);
}
