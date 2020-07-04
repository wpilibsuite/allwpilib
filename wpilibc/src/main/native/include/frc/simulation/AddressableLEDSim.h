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

#include <hal/simulation/AddressableLEDData.h>
#include <wpi/ArrayRef.h>

#include "CallbackStore.h"
#include "frc/AddressableLED.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated addressable LED.
 */
class AddressableLEDSim {
 public:
  /**
   * Constructs for the first addressable LED.
   */
  AddressableLEDSim() : m_index{0} {}

  /**
   * Constructs from an AddressableLED object.
   *
   * @param addressableLED AddressableLED to simulate
   */
  explicit AddressableLEDSim(const AddressableLED& addressableLED)
      : m_index{0} {}

  /**
   * Creates an AddressableLEDSim for a PWM channel.
   *
   * @param pwmChannel PWM channel
   * @return Simulated object
   * @throws std::out_of_range if no AddressableLED is configured for that
   *         channel
   */
  static AddressableLEDSim CreateForChannel(int pwmChannel) {
    int index = HALSIM_FindAddressableLEDForChannel(pwmChannel);
    if (index < 0)
      throw std::out_of_range("no addressable LED found for PWM channel");
    return AddressableLEDSim{index};
  }

  /**
   * Creates an AddressableLEDSim for a simulated index.
   * The index is incremented for each simulated AddressableLED.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static AddressableLEDSim CreateForIndex(int index) {
    return AddressableLEDSim{index};
  }

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAddressableLEDInitializedCallback);
    store->SetUid(HALSIM_RegisterAddressableLEDInitializedCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitialized() const {
    return HALSIM_GetAddressableLEDInitialized(m_index);
  }

  void SetInitialized(bool initialized) {
    HALSIM_SetAddressableLEDInitialized(m_index, initialized);
  }

  std::unique_ptr<CallbackStore> registerOutputPortCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAddressableLEDOutputPortCallback);
    store->SetUid(HALSIM_RegisterAddressableLEDOutputPortCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetOutputPort() const {
    return HALSIM_GetAddressableLEDOutputPort(m_index);
  }

  void SetOutputPort(int outputPort) {
    HALSIM_SetAddressableLEDOutputPort(m_index, outputPort);
  }

  std::unique_ptr<CallbackStore> RegisterLengthCallback(NotifyCallback callback,
                                                        bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAddressableLEDLengthCallback);
    store->SetUid(HALSIM_RegisterAddressableLEDLengthCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetLength() const { return HALSIM_GetAddressableLEDLength(m_index); }

  void SetLength(int length) {
    HALSIM_SetAddressableLEDLength(m_index, length);
  }

  std::unique_ptr<CallbackStore> RegisterRunningCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAddressableLEDRunningCallback);
    store->SetUid(HALSIM_RegisterAddressableLEDRunningCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  int GetRunning() const { return HALSIM_GetAddressableLEDRunning(m_index); }

  void SetRunning(bool running) {
    HALSIM_SetAddressableLEDRunning(m_index, running);
  }

  std::unique_ptr<CallbackStore> RegisterDataCallback(NotifyCallback callback,
                                                      bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelAddressableLEDDataCallback);
    store->SetUid(HALSIM_RegisterAddressableLEDDataCallback(
        m_index, &ConstBufferCallbackStoreThunk, store.get()));
    return store;
  }

  int GetData(struct HAL_AddressableLEDData* data) const {
    return HALSIM_GetAddressableLEDData(m_index, data);
  }

  void SetData(struct HAL_AddressableLEDData* data, int length) {
    HALSIM_SetAddressableLEDData(m_index, data, length);
  }

 private:
  explicit AddressableLEDSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
