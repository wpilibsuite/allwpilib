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

#include <hal/simulation/DigitalPWMData.h>

#include "CallbackStore.h"
#include "frc/DigitalOutput.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated digital PWM output.
 *
 * This is for duty cycle PWM outputs on a DigitalOutput, not for the servo
 * style PWM outputs on a PWM channel.
 */
class DigitalPWMSim {
 public:
  /**
   * Constructs from a DigitalOutput object.
   *
   * @param digitalOutput DigitalOutput to simulate
   */
  explicit DigitalPWMSim(const DigitalOutput& digitalOutput)
      : m_index{digitalOutput.GetChannel()} {}

  /**
   * Creates an DigitalPWMSim for a digital I/O channel.
   *
   * @param channel DIO channel
   * @return Simulated object
   * @throws std::out_of_range if no Digital PWM is configured for that channel
   */
  static DigitalPWMSim CreateForChannel(int channel) {
    int index = HALSIM_FindDigitalPWMForChannel(channel);
    if (index < 0) throw std::out_of_range("no digital PWM found for channel");
    return DigitalPWMSim{index};
  }

  /**
   * Creates an DigitalPWMSim for a simulated index.
   * The index is incremented for each simulated DigitalPWM.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static DigitalPWMSim CreateForIndex(int index) {
    return DigitalPWMSim{index};
  }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMInitializedCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetDigitalPWMInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetDigitalPWMInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> RegisterDutyCycleCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMDutyCycleCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMDutyCycleCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  double GetDutyCycle() const { return HALSIM_GetDigitalPWMDutyCycle(m_index); }

  void SetDutyCycle(double dutyCycle) {
    HALSIM_SetDigitalPWMDutyCycle(m_index, dutyCycle);
  }

  std::unique_ptr<CallbackStore> RegisterPinCallback(NotifyCallback callback,
                                                     bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelDigitalPWMPinCallback);
    store->SetUid(HALSIM_RegisterDigitalPWMPinCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetPin() const { return HALSIM_GetDigitalPWMPin(m_index); }

  void SetPin(int pin) { HALSIM_SetDigitalPWMPin(m_index, pin); }

  void ResetData() { HALSIM_ResetDigitalPWMData(m_index); }

 private:
  explicit DigitalPWMSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
