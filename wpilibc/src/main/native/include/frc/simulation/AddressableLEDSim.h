// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

struct HAL_AddressableLEDData;

namespace frc {

class AddressableLED;

namespace sim {

/**
 * Class to control a simulated addressable LED.
 */
class AddressableLEDSim {
 public:
  /**
   * Constructs for the first addressable LED.
   */
  AddressableLEDSim();

  /**
   * Constructs from an AddressableLED object.
   *
   * @param addressableLED AddressableLED to simulate
   */
  explicit AddressableLEDSim(const AddressableLED& addressableLED);

  /**
   * Creates an AddressableLEDSim for a PWM channel.
   *
   * @param pwmChannel PWM channel
   * @return Simulated object
   * @throws std::out_of_range if no AddressableLED is configured for that
   *         channel
   */
  static AddressableLEDSim CreateForChannel(int pwmChannel);

  /**
   * Creates an AddressableLEDSim for a simulated index.
   * The index is incremented for each simulated AddressableLED.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static AddressableLEDSim CreateForIndex(int index);

  /**
   * Register a callback on the Initialized property.
   *
   * @param callback the callback that will be called whenever the Initialized
   *                 property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object storing this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if initialized.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Change the Initialized value of the LED strip.
   *
   * @param initialized the new value
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback on the output port.
   *
   * @param callback the callback that will be called whenever the output port
   *                 is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterOutputPortCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the output port.
   *
   * @return the output port
   */
  int GetOutputPort() const;

  /**
   * Change the output port.
   *
   * @param outputPort the new output port
   */
  void SetOutputPort(int outputPort);

  /**
   * Register a callback on the length.
   *
   * @param callback the callback that will be called whenever the length is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterLengthCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the length of the LED strip.
   *
   * @return the length
   */
  int GetLength() const;

  /**
   * Change the length of the LED strip.
   *
   * @param length the new value
   */
  void SetLength(int length);

  /**
   * Register a callback on whether the LEDs are running.
   *
   * @param callback the callback that will be called whenever the LED state is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterRunningCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the LEDs are running.
   *
   * @return true if they are
   */
  int GetRunning() const;

  /**
   * Change whether the LEDs are active.
   *
   * @param running the new value
   */
  void SetRunning(bool running);

  /**
   * Register a callback on the LED data.
   *
   * @param callback the callback that will be called whenever the LED data is
   *                 changed
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterDataCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the LED data.
   *
   * @param data output parameter to fill with LED data
   * @return the length of the LED data
   */
  int GetData(struct HAL_AddressableLEDData* data) const;

  /**
   * Change the LED data.
   *
   * @param data the new data
   * @param length the length of the LED data
   */
  void SetData(struct HAL_AddressableLEDData* data, int length);

 private:
  explicit AddressableLEDSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
