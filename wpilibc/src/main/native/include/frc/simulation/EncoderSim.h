// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

  /**
   * Register a callback on the Initialized property of the encoder.
   *
   * @param callback the callback that will be called whenever the Initialized
   *                 property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterInitializedCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the Initialized value of the encoder.
   *
   * @return true if initialized
   */
  bool GetInitialized() const;

  /**
   * Change the Initialized value of the encoder.
   *
   * @param initialized the new value
   */
  void SetInitialized(bool initialized);

  /**
   * Register a callback on the count property of the encoder.
   *
   * @param callback the callback that will be called whenever the count
   *                 property is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterCountCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the count of the encoder.
   *
   * @return the count
   */
  int GetCount() const;

  /**
   * Change the count of the encoder.
   *
   * @param count the new count
   */
  void SetCount(int count);

  /**
   * Register a callback on the period of the encoder.
   *
   * @param callback the callback that will be called whenever the period is
   *                 changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterPeriodCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the period of the encoder.
   *
   * @return the encoder period
   */
  double GetPeriod() const;

  /**
   * Change the encoder period.
   *
   * @param period the new period
   */
  void SetPeriod(double period);

  /**
   * Register a callback to be called whenever the encoder is reset.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterResetCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Check if the encoder has been reset.
   *
   * @return true if reset
   */
  bool GetReset() const;

  /**
   * Change the reset property of the encoder.
   *
   * @param reset the new value
   */
  void SetReset(bool reset);

  /**
   * Register a callback to be run whenever the max period of the encoder is
   * changed.
   *
   * @param callback the callback
   * @param initialNotify whether to run the callback on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterMaxPeriodCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the max period of the encoder.
   *
   * @return the max period of the encoder
   */
  double GetMaxPeriod() const;

  /**
   * Change the max period of the encoder.
   *
   * @param maxPeriod the new value
   */
  void SetMaxPeriod(double maxPeriod);

  /**
   * Register a callback on the direction of the encoder.
   *
   * @param callback the callback that will be called whenever the direction
   *                 is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterDirectionCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the direction of the encoder.
   *
   * @return the direction of the encoder
   */
  bool GetDirection() const;

  /**
   * Set the direction of the encoder.
   *
   * @param direction the new direction
   */
  void SetDirection(bool direction);

  /**
   * Register a callback on the reverse direction.
   *
   * @param callback the callback that will be called whenever the reverse
   *                 direction is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterReverseDirectionCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the reverse direction of the encoder.
   *
   * @return the reverse direction of the encoder
   */
  bool GetReverseDirection() const;

  /**
   * Set the reverse direction.
   *
   * @param reverseDirection the new value
   */
  void SetReverseDirection(bool reverseDirection);

  /**
   * Register a callback on the samples-to-average value of this encoder.
   *
   * @param callback the callback that will be called whenever the
   *                 samples-to-average is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterSamplesToAverageCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Get the samples-to-average value.
   *
   * @return the samples-to-average value
   */
  int GetSamplesToAverage() const;

  /**
   * Set the samples-to-average value.
   *
   * @param samplesToAverage the new value
   */
  void SetSamplesToAverage(int samplesToAverage);

  /**
   * Register a callback on the distance per pulse value of this encoder.
   *
   * @param callback the callback that will be called whenever the
   *                 distance per pulse is changed
   * @param initialNotify if true, the callback will be run on the initial value
   * @return the CallbackStore object associated with this callback
   */
  [[nodiscard]] std::unique_ptr<CallbackStore> RegisterDistancePerPulseCallback(
      NotifyCallback callback, bool initialNotify);

  /**
   * Read the distance per pulse of the encoder.
   *
   * @return the encoder distance per pulse
   */
  double GetDistancePerPulse() const;

  /**
   * Change the encoder distance per pulse.
   *
   * @param distancePerPulse the new distance per pulse
   */
  void SetDistancePerPulse(double distancePerPulse);

  /**
   * Resets all simulation data for this encoder.
   */
  void ResetData();

  /**
   * Change the encoder distance.
   *
   * @param distance the new distance
   */
  void SetDistance(double distance);

  /**
   * Read the distance of the encoder.
   *
   * @return the encoder distance
   */
  double GetDistance();

  /**
   * Change the rate of the encoder.
   *
   * @param rate the new rate
   */
  void SetRate(double rate);

  /**
   * Get the rate of the encoder.
   *
   * @return the rate of change
   */
  double GetRate();

 private:
  explicit EncoderSim(int index) : m_index{index} {}

  int m_index;
};
}  // namespace sim
}  // namespace frc
