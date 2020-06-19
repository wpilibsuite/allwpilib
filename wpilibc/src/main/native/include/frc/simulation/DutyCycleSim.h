/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <exception>
#include <memory>
#include <utility>

#include <hal/simulation/DutyCycleData.h>

#include "CallbackStore.h"
#include "frc/DutyCycle.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated duty cycle digital input.
 */
class DutyCycleSim {
 public:
  /**
   * Constructs from a DutyCycle object.
   *
   * @param dutyCycle DutyCycle to simulate
   */
  explicit DutyCycleSim(const DutyCycle& dutyCycle)
      : m_index{dutyCycle.GetFPGAIndex()} {}

  /**
   * Creates a DutyCycleSim for a digital input channel.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws std::out_of_range if no DutyCycle is configured for that channel
   */
  static DutyCycleSim CreateForChannel(int channel) {
    int index = HALSIM_FindDutyCycleForChannel(channel);
    if (index < 0) throw std::out_of_range("no duty cycle found for channel");
    return DutyCycleSim{index};
  }

  /**
   * Creates a DutyCycleSim for a simulated index.
   * The index is incremented for each simulated DutyCycle.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DutyCycleSim CreateForIndex(int index) { return DutyCycleSim{index}; }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDutyCycleInitializedCallback);
    store->SetUid(HALSIM_RegisterDutyCycleInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetDutyCycleInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetDutyCycleInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterFrequencyCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDutyCycleFrequencyCallback);
    store->SetUid(HALSIM_RegisterDutyCycleFrequencyCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetFrequency() const { return HALSIM_GetDutyCycleFrequency(m_index); }

  void SetFrequency(int count) { HALSIM_SetDutyCycleFrequency(m_index, count); }

  std::unique_ptr<CallbackStore> RegisterOutputCallback(NotifyCallback callback,
                                                        bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDutyCycleOutputCallback);
    store->SetUid(HALSIM_RegisterDutyCycleOutputCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetOutput() const { return HALSIM_GetDutyCycleOutput(m_index); }

  void SetOutput(double period) { HALSIM_SetDutyCycleOutput(m_index, period); }

  void ResetData() { HALSIM_ResetDutyCycleData(m_index); }

 private:
  explicit DutyCycleSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
