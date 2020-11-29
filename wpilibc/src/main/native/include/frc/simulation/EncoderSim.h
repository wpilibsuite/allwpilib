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

class Encoder;

namespace sim {

/**
 * Class to control a simulated encoder.
 */
class EncoderSim {
 public:
  /**
   * Constructs from an Encoder object.
   *
   * @param encoder Encoder to simulate
   */
  explicit EncoderSim(const Encoder& encoder);

  /**
   * Creates an EncoderSim for a digital input channel.  Encoders take two
   * channels, so either one may be specified.
   *
   * @param channel digital input channel
   * @return Simulated object
   * @throws NoSuchElementException if no Encoder is configured for that channel
   */
  static EncoderSim CreateForChannel(int channel);

  /**
   * Creates an EncoderSim for a simulated index.
   * The index is incremented for each simulated Encoder.
   *
   * @param index simulator index
   * @return Simulated object
   */
  static EncoderSim CreateForIndex(int index);

  void setDisplayName(const char* displayName);
  const char* getDisplayName();

  std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetInitialized() const;

  void SetInitialized(bool initialized);

  std::unique_ptr<CallbackStore> RegisterCountCallback(NotifyCallback callback,
                                                       bool initialNotify);

  int GetCount() const;

  void SetCount(int count);

  std::unique_ptr<CallbackStore> RegisterPeriodCallback(NotifyCallback callback,
                                                        bool initialNotify);

  double GetPeriod() const;

  void SetPeriod(double period);

  std::unique_ptr<CallbackStore> RegisterResetCallback(NotifyCallback callback,
                                                       bool initialNotify);

  bool GetReset() const;

  void SetReset(bool reset);

  std::unique_ptr<CallbackStore> RegisterMaxPeriodCallback(
      NotifyCallback callback, bool initialNotify);

  double GetMaxPeriod() const;

  void SetMaxPeriod(double maxPeriod);

  std::unique_ptr<CallbackStore> RegisterDirectionCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetDirection() const;

  void SetDirection(bool direction);

  std::unique_ptr<CallbackStore> RegisterReverseDirectionCallback(
      NotifyCallback callback, bool initialNotify);

  bool GetReverseDirection() const;

  void SetReverseDirection(bool reverseDirection);

  std::unique_ptr<CallbackStore> RegisterSamplesToAverageCallback(
      NotifyCallback callback, bool initialNotify);

  int GetSamplesToAverage() const;

  void SetSamplesToAverage(int samplesToAverage);

  std::unique_ptr<CallbackStore> RegisterDistancePerPulseCallback(
      NotifyCallback callback, bool initialNotify);

  double GetDistancePerPulse() const;

  void SetDistancePerPulse(double distancePerPulse);

  void ResetData();

  void SetDistance(double distance);

  double GetDistance();

  void SetRate(double rate);

  double GetRate();

 private:
  explicit EncoderSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
