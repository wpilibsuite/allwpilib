/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <exception>
#include <memory>
#include <utility>

#include <hal/simulation/AnalogTriggerData.h>

#include "CallbackStore.h"
#include "frc/AnalogTrigger.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated analog trigger.
 */
class AnalogTriggerSim {
 public:
  /**
   * Constructs from an AnalogTrigger object.
   *
   * @param analogTrigger AnalogTrigger to simulate
   */
  explicit AnalogTriggerSim(const AnalogTrigger& analogTrigger)
      : m_index{analogTrigger.GetIndex()} {}

  /**
   * Creates an AnalogTriggerSim for an analog input channel.
   *
   * @param channel analog input channel
   * @return Simulated object
   * @throws std::out_of_range if no AnalogTrigger is configured for that
   *         channel
   */
  static AnalogTriggerSim CreateForChannel(int channel) {
    int index = HALSIM_FindAnalogTriggerForChannel(channel);
    if (index < 0)
      throw std::out_of_range("no analog trigger found for channel");
    return AnalogTriggerSim{index};
  }

  /**
   * Creates an AnalogTriggerSim for a simulated index.
   * The index is incremented for each simulated AnalogTrigger.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static AnalogTriggerSim CreateForIndex(int index) {
    return AnalogTriggerSim{index};
  }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAnalogTriggerInitializedCallback);
    store->SetUid(HALSIM_RegisterAnalogTriggerInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetAnalogTriggerInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetAnalogTriggerInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterTriggerLowerBoundCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback,
        &HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback);
    store->SetUid(HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetTriggerLowerBound() const {
    return HALSIM_GetAnalogTriggerTriggerLowerBound(m_index);
  }

  void SetTriggerLowerBound(double triggerLowerBound) {
    HALSIM_SetAnalogTriggerTriggerLowerBound(m_index, triggerLowerBound);
  }

  std::unique_ptr<CallbackStore> RegisterTriggerUpperBoundCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback,
        &HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback);
    store->SetUid(HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetTriggerUpperBound() const {
    return HALSIM_GetAnalogTriggerTriggerUpperBound(m_index);
  }

  void SetTriggerUpperBound(double triggerUpperBound) {
    HALSIM_SetAnalogTriggerTriggerUpperBound(m_index, triggerUpperBound);
  }

  void ResetData() { HALSIM_ResetAnalogTriggerData(m_index); }

 private:
  explicit AnalogTriggerSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
