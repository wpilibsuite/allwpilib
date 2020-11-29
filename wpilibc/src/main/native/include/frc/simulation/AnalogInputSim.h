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

class AnalogInput;

namespace sim {

/**
 * Class to control a simulated analog input.
 */
class AnalogInputSim {
 public:
  /**
   * Constructs from an AnalogInput object.
   *
   * @param analogInput AnalogInput to simulate
   */
  explicit AnalogInputSim(const AnalogInput& analogInput);

  /**
   * Constructs from an analog input channel number.
   *
   * @param channel Channel number
   */
  explicit AnalogInputSim(int channel);

  void setDisplayName(const char* displayName);
  const char* getDisplayName();

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterAverageBitsCallback(
      NotifyCallback callback, bool initialNotify);

  int GetAverageBits() const;

  void SetAverageBits(int averageBits);

  std::unique_ptr<CallbackStore> RegisterOversampleBitsCallback(
      NotifyCallback callback, bool initialNotify);

  int GetOversampleBits() const;

  void SetOversampleBits(int oversampleBits);

  std::unique_ptr<CallbackStore> RegisterVoltageCallback(
      NotifyCallback callback, bool initialNotify);

  double GetVoltage() const;

  void SetVoltage(double voltage);

  std::unique_ptr<CallbackStore> RegisterAccumulatorInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetAccumulatorInitialized() const;

  void SetAccumulatorInitialized(bool accumulatorInitialized);

  std::unique_ptr<CallbackStore> RegisterAccumulatorValueCallback(
      NotifyCallback callback, bool initialNotify);

  int64_t GetAccumulatorValue() const;

  void SetAccumulatorValue(int64_t accumulatorValue);

  std::unique_ptr<CallbackStore> RegisterAccumulatorCountCallback(
      NotifyCallback callback, bool initialNotify);

  int64_t GetAccumulatorCount() const;

  void SetAccumulatorCount(int64_t accumulatorCount);

  std::unique_ptr<CallbackStore> RegisterAccumulatorCenterCallback(
      NotifyCallback callback, bool initialNotify);

  int GetAccumulatorCenter() const;

  void SetAccumulatorCenter(int accumulatorCenter);

  std::unique_ptr<CallbackStore> RegisterAccumulatorDeadbandCallback(
      NotifyCallback callback, bool initialNotify);

  int GetAccumulatorDeadband() const;

  void SetAccumulatorDeadband(int accumulatorDeadband);

  void ResetData();

 private:
  int m_index;
};
}  // namespace sim
}  // namespace frc
