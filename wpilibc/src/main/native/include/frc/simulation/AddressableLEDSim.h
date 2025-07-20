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
   * Constructs an addressable LED for a specific channel.
   *
   * @param channel output channel
   */
  explicit AddressableLEDSim(int channel);

  /**
   * Constructs from an AddressableLED object.
   *
   * @param addressableLED AddressableLED to simulate
   */
  explicit AddressableLEDSim(const AddressableLED& addressableLED);

  /**
   * Register a callback on the Initialized property.
   *
   * @param callback the callback that will be called whenever the Initialized
   *                 property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object storing this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
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
   * Register a callback on the start.
   *
   * @param callback the callback that will be called whenever the start
   *                 is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterStartCallback(NotifyCallback callback,
                                                       bool initialNotify);

  /**
   * Get the start.
   *
   * @return the start
   */
  int GetStart() const;

  /**
   * Change the start.
   *
   * @param start the new start
   */
  void SetStart(int start);

  /**
   * Register a callback on the length.
   *
   * @param callback the callback that will be called whenever the length is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  std::unique_ptr<CallbackStore> RegisterLengthCallback(NotifyCallback callback,
                                                        bool initialNotify);

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
   */
  void SetData(struct HAL_AddressableLEDData* data);

  /**
   * Register a callback on the LED data.
   *
   * @param callback the callback that will be called whenever the LED data is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]]
  static std::unique_ptr<CallbackStore> RegisterDataCallback(
      ConstBufferCallback callback, bool initialNotify);

  /**
   * Get the global LED data.
   *
   * @param start the start of the LED data
   * @param length the length of the LED data
   * @param data output parameter to fill with LED data
   * @return the length of the LED data
   */
  static int GetGlobalData(int start, int length,
                           struct HAL_AddressableLEDData* data);

  /**
   * Change the global LED data.
   *
   * @param start the start of the LED data
   * @param length the length of the LED data
   * @param data the new data
   */
  static void SetGlobalData(int start, int length,
                            struct HAL_AddressableLEDData* data);

 private:
  int m_channel;
};
}  // namespace sim
}  // namespace frc
