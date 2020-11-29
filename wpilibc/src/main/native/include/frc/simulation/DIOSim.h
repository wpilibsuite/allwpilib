/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/simulation/CallbackStore.h"

namespace frc {

class DigitalInput;
class DigitalOutput;

namespace sim {

/**
 * Class to control a simulated digital input or output.
 */
class DIOSim {
 public:
  /**
   * Constructs from a DigitalInput object.
   *
   * @param input DigitalInput to simulate
   */
  explicit DIOSim(const DigitalInput& input);

  /**
   * Constructs from a DigitalOutput object.
   *
   * @param output DigitalOutput to simulate
   */
  explicit DIOSim(const DigitalOutput& output);

  /**
   * Constructs from an digital I/O channel number.
   *
   * @param channel Channel number
   */
  explicit DIOSim(int channel);

  void setDisplayName(const char* displayName);
  const char* getDisplayName();

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterValueCallback(NotifyCallback callback,
                                                       bool initialNotify);

  bool GetValue() const;

  void SetValue(bool value);

  std::unique_ptr<CallbackStore> RegisterPulseLengthCallback(
      NotifyCallback callback, bool initialNotify);

  double GetPulseLength() const;

  void SetPulseLength(double pulseLength);

  std::unique_ptr<CallbackStore> RegisterIsInputCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetIsInput() const;

  void SetIsInput(bool isInput);

  std::unique_ptr<CallbackStore> RegisterFilterIndexCallback(
      NotifyCallback callback, bool initialNotify);

  int GetFilterIndex() const;

  void SetFilterIndex(int filterIndex);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
