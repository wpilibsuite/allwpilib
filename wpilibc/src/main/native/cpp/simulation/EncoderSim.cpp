// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/EncoderSim.h"

#include <memory>
#include <stdexcept>

#include <hal/simulation/EncoderData.h>

#include "frc/Encoder.h"

using namespace frc;
using namespace frc::sim;

EncoderSim::EncoderSim(const Encoder& encoder)
    : m_index{encoder.GetFPGAIndex()} {}

EncoderSim EncoderSim::CreateForChannel(int channel) {
  int index = HALSIM_FindEncoderForChannel(channel);
  if (index < 0) {
    throw std::out_of_range("no encoder found for channel");
  }
  return EncoderSim{index};
}

EncoderSim EncoderSim::CreateForIndex(int index) {
  return EncoderSim{index};
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterInitializedCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderInitializedCallback);
  store->SetUid(HALSIM_RegisterEncoderInitializedCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool EncoderSim::GetInitialized() const {
  return HALSIM_GetEncoderInitialized(m_index);
}

void EncoderSim::SetInitialized(bool initialized) {
  HALSIM_SetEncoderInitialized(m_index, initialized);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterCountCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderCountCallback);
  store->SetUid(HALSIM_RegisterEncoderCountCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int EncoderSim::GetCount() const {
  return HALSIM_GetEncoderCount(m_index);
}

void EncoderSim::SetCount(int count) {
  HALSIM_SetEncoderCount(m_index, count);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterPeriodCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderPeriodCallback);
  store->SetUid(HALSIM_RegisterEncoderPeriodCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double EncoderSim::GetPeriod() const {
  return HALSIM_GetEncoderPeriod(m_index);
}

void EncoderSim::SetPeriod(double period) {
  HALSIM_SetEncoderPeriod(m_index, period);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterResetCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderResetCallback);
  store->SetUid(HALSIM_RegisterEncoderResetCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool EncoderSim::GetReset() const {
  return HALSIM_GetEncoderReset(m_index);
}

void EncoderSim::SetReset(bool reset) {
  HALSIM_SetEncoderReset(m_index, reset);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterMaxPeriodCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderMaxPeriodCallback);
  store->SetUid(HALSIM_RegisterEncoderMaxPeriodCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double EncoderSim::GetMaxPeriod() const {
  return HALSIM_GetEncoderMaxPeriod(m_index);
}

void EncoderSim::SetMaxPeriod(double maxPeriod) {
  HALSIM_SetEncoderMaxPeriod(m_index, maxPeriod);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterDirectionCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderDirectionCallback);
  store->SetUid(HALSIM_RegisterEncoderDirectionCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool EncoderSim::GetDirection() const {
  return HALSIM_GetEncoderDirection(m_index);
}

void EncoderSim::SetDirection(bool direction) {
  HALSIM_SetEncoderDirection(m_index, direction);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterReverseDirectionCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderReverseDirectionCallback);
  store->SetUid(HALSIM_RegisterEncoderReverseDirectionCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

bool EncoderSim::GetReverseDirection() const {
  return HALSIM_GetEncoderReverseDirection(m_index);
}

void EncoderSim::SetReverseDirection(bool reverseDirection) {
  HALSIM_SetEncoderReverseDirection(m_index, reverseDirection);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterSamplesToAverageCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderSamplesToAverageCallback);
  store->SetUid(HALSIM_RegisterEncoderSamplesToAverageCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

int EncoderSim::GetSamplesToAverage() const {
  return HALSIM_GetEncoderSamplesToAverage(m_index);
}

void EncoderSim::SetSamplesToAverage(int samplesToAverage) {
  HALSIM_SetEncoderSamplesToAverage(m_index, samplesToAverage);
}

std::unique_ptr<CallbackStore> EncoderSim::RegisterDistancePerPulseCallback(
    NotifyCallback callback, bool initialNotify) {
  auto store = std::make_unique<CallbackStore>(
      m_index, -1, callback, &HALSIM_CancelEncoderDistancePerPulseCallback);
  store->SetUid(HALSIM_RegisterEncoderDistancePerPulseCallback(
      m_index, &CallbackStoreThunk, store.get(), initialNotify));
  return store;
}

double EncoderSim::GetDistancePerPulse() const {
  return HALSIM_GetEncoderDistancePerPulse(m_index);
}

void EncoderSim::SetDistancePerPulse(double distancePerPulse) {
  HALSIM_SetEncoderDistancePerPulse(m_index, distancePerPulse);
}

void EncoderSim::ResetData() {
  HALSIM_ResetEncoderData(m_index);
}

void EncoderSim::SetDistance(double distance) {
  HALSIM_SetEncoderDistance(m_index, distance);
}

double EncoderSim::GetDistance() const {
  return HALSIM_GetEncoderDistance(m_index);
}

void EncoderSim::SetRate(double rate) {
  HALSIM_SetEncoderRate(m_index, rate);
}

double EncoderSim::GetRate() const {
  return HALSIM_GetEncoderRate(m_index);
}
