// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/angle.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"

namespace frc {
class DutyCycle;
class DigitalSource;

/**
 * Class for supporting duty cycle/PWM encoders, such as the US Digital MA3 with
 * PWM Output, the CTRE Mag Encoder, the Rev Hex Encoder, and the AM Mag
 * Encoder.
 */
class DutyCycleEncoder : public wpi::Sendable,
                         public wpi::SendableHelper<DutyCycleEncoder> {
 public:
  /**
   * Construct a new DutyCycleEncoder on a specific channel.
   *
   * @param channel the channel to attach to
   */
  explicit DutyCycleEncoder(int channel);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(DutyCycle& dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(DutyCycle* dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(DigitalSource& digitalSource);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(DigitalSource* digitalSource);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource);

  ~DutyCycleEncoder() override = default;

  DutyCycleEncoder(DutyCycleEncoder&&) = default;
  DutyCycleEncoder& operator=(DutyCycleEncoder&&) = default;

  /**
   * Get the frequency in Hz of the duty cycle signal from the encoder.
   *
   * @return duty cycle frequency in Hz
   */
  int GetFrequency() const;

  /**
   * Get if the sensor is connected
   *
   * This uses the duty cycle frequency to determine if the sensor is connected.
   * By default, a value of 100 Hz is used as the threshold, and this value can
   * be changed with SetConnectedFrequencyThreshold.
   *
   * @return true if the sensor is connected
   */
  bool IsConnected() const;

  /**
   * Change the frequency threshold for detecting connection used by
   * IsConnected.
   *
   * @param frequency the minimum frequency in Hz.
   */
  void SetConnectedFrequencyThreshold(int frequency);

  /**
   * Reset the Encoder distance to zero.
   */
  void Reset();

  /**
   * Get the encoder value since the last reset.
   *
   * This is reported in rotations since the last reset.
   *
   * @return the encoder value in rotations
   */
  units::turn_t Get() const;

  /**
   * Get the absolute position of the duty cycle encoder encoder.
   *
   * <p>GetAbsolutePosition() - GetPositionOffset() will give an encoder
   * absolute position relative to the last reset. This could potentially be
   * negative, which needs to be accounted for.
   *
   * <p>This will not account for rollovers, and will always be just the raw
   * absolute position.
   *
   * @return the absolute position
   */
  double GetAbsolutePosition() const;

  /**
   * Get the offset of position relative to the last reset.
   *
   * GetAbsolutePosition() - GetPositionOffset() will give an encoder absolute
   * position relative to the last reset. This could potentially be negative,
   * which needs to be accounted for.
   *
   * @return the position offset
   */
  double GetPositionOffset() const;

  /**
   * Set the position offset.
   *
   * <p>This must be in the range of 0-1.
   *
   * @param offset the offset
   */
  void SetPositionOffset(double offset);

  /**
   * Set the encoder duty cycle range. As the encoder needs to maintain a duty
   * cycle, the duty cycle cannot go all the way to 0% or all the way to 100%.
   * For example, an encoder with a 4096 us period might have a minimum duty
   * cycle of 1 us / 4096 us and a maximum duty cycle of 4095 / 4096 us. Setting
   * the range will result in an encoder duty cycle less than or equal to the
   * minimum being output as 0 rotation, the duty cycle greater than or equal to
   * the maximum being output as 1 rotation, and values in between linearly
   * scaled from 0 to 1.
   *
   * @param min minimum duty cycle (0-1 range)
   * @param max maximum duty cycle (0-1 range)
   */
  void SetDutyCycleRange(double min, double max);

  /**
   * Set the distance per rotation of the encoder. This sets the multiplier used
   * to determine the distance driven based on the rotation value from the
   * encoder. Set this value based on the how far the mechanism travels in 1
   * rotation of the encoder, and factor in gearing reductions following the
   * encoder shaft. This distance can be in any units you like, linear or
   * angular.
   *
   * @param distancePerRotation the distance per rotation of the encoder
   */
  void SetDistancePerRotation(double distancePerRotation);

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful
   * units.
   */
  double GetDistancePerRotation() const;

  /**
   * Get the distance the sensor has driven since the last reset as scaled by
   * the value from SetDistancePerRotation.
   *
   * @return The distance driven since the last reset
   */
  double GetDistance() const;

  /**
   * Get the FPGA index for the DutyCycleEncoder.
   *
   * @return the FPGA index
   */
  int GetFPGAIndex() const;

  /**
   * Get the channel of the source.
   *
   * @return the source channel
   */
  int GetSourceChannel() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void Init();
  double MapSensorRange(double pos) const;

  std::shared_ptr<DutyCycle> m_dutyCycle;
  std::unique_ptr<AnalogTrigger> m_analogTrigger;
  std::unique_ptr<Counter> m_counter;
  int m_frequencyThreshold = 100;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
  mutable units::turn_t m_lastPosition{0.0};
  double m_sensorMin = 0;
  double m_sensorMax = 1;

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
  hal::SimDouble m_simAbsolutePosition;
  hal::SimDouble m_simDistancePerRotation;
  hal::SimBoolean m_simIsConnected;
};
}  // namespace frc
