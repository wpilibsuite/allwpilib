/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <hal/simulation/AnalogGyroData.h>

#include "CallbackStore.h"
#include "frc/AnalogGyro.h"
#include "frc/AnalogInput.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated analog gyro.
 */
class AnalogGyroSim {
 public:
  /**
   * Constructs from an AnalogGyro object.
   *
   * @param gyro AnalogGyro to simulate
   */
  explicit AnalogGyroSim(const AnalogGyro& gyro)
      : m_index{gyro.GetAnalogInput()->GetChannel()} {}

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  explicit AnalogGyroSim(int channel) : m_index{channel} {}

  std::unique_ptr<CallbackStore> RegisterAngleCallback(NotifyCallback callback,
                                                       bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogGyroAngleCallback);
    store->SetUid(HALSIM_RegisterAnalogGyroAngleCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetAngle() const { return HALSIM_GetAnalogGyroAngle(m_index); }

  void SetAngle(double angle) { HALSIM_SetAnalogGyroAngle(m_index, angle); }

  std::unique_ptr<CallbackStore> RegisterRateCallback(NotifyCallback callback,
                                                      bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogGyroRateCallback);
    store->SetUid(HALSIM_RegisterAnalogGyroRateCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetRate() const { return HALSIM_GetAnalogGyroRate(m_index); }

  void SetRate(double rate) { HALSIM_SetAnalogGyroRate(m_index, rate); }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogGyroInitializedCallback);
    store->SetUid(HALSIM_RegisterAnalogGyroInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetAnalogGyroInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetAnalogGyroInitialized(m_index, initialized);
  }

  void ResetData() { HALSIM_ResetAnalogGyroData(m_index); }

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
