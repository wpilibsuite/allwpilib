/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterOutputPortCallback(
      NotifyCallback callback, bool initialNotify);

  int GetOutputPort() const;

  void SetOutputPort(int outputPort);

  std::unique_ptr<CallbackStore> RegisterLengthCallback(NotifyCallback callback,
                                                        bool initialNotify);

  int GetLength() const;

  void SetLength(int length);

  std::unique_ptr<CallbackStore> RegisterRunningCallback(
      NotifyCallback callback, bool initialNotify);

  int GetRunning() const;

  void SetRunning(bool running);

  std::unique_ptr<CallbackStore> RegisterDataCallback(NotifyCallback callback,
                                                      bool initialNotify);

  int GetData(struct HAL_AddressableLEDData* data) const;

  void SetData(struct HAL_AddressableLEDData* data, int length);

 private:
  explicit AddressableLEDSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
