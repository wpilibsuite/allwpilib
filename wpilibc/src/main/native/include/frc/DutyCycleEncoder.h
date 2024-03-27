// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/frequency.h>
#include <units/time.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

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
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param channel the channel to attach to
   */
  explicit DutyCycleEncoder(int channel);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(DutyCycle& dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(DutyCycle* dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param dutyCycle the duty cycle to attach to
   */
  explicit DutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(DigitalSource& digitalSource);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(DigitalSource* digitalSource);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param digitalSource the digital source to attach to
   */
  explicit DutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource);

  /**
   * Construct a new DutyCycleEncoder on a specific channel.
   *
   * @param channel the channel to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(int channel, double fullRange, double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(DutyCycle& dutyCycle, double fullRange, double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(DutyCycle* dutyCycle, double fullRange, double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to an existing DutyCycle object.
   *
   * @param dutyCycle the duty cycle to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(std::shared_ptr<DutyCycle> dutyCycle, double fullRange,
                   double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(DigitalSource& digitalSource, double fullRange,
                   double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(DigitalSource* digitalSource, double fullRange,
                   double expectedZero);

  /**
   * Construct a new DutyCycleEncoder attached to a DigitalSource object.
   *
   * @param digitalSource the digital source to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  DutyCycleEncoder(std::shared_ptr<DigitalSource> digitalSource,
                   double fullRange, double expectedZero);

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
   * Get the encoder value.
   *
   * @return the encoder value scaled by the full range input
   */
  double Get() const;

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
   * Sets the assumed frequency of the connected device.
   *
   * <p>By default, the DutyCycle engine has to compute the frequency of the
   * input signal. This can result in both delayed readings and jumpy readings.
   * To solve this, you can pass the expected frequency of the sensor to this
   * function. This will use that frequency to compute the DutyCycle percentage,
   * rather than the computed frequency.
   *
   * @param frequency the assumed frequency of the sensor
   */
  void SetAssumedFrequency(units::hertz_t frequency);

  /**
   * Set if this encoder is inverted.
   *
   * @param inverted true to invert the encoder, false otherwise
   */
  void SetInverted(bool inverted);

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
  void Init(double fullRange, double expectedZero);
  double MapSensorRange(double pos) const;

  std::shared_ptr<DutyCycle> m_dutyCycle;
  int m_frequencyThreshold = 100;
  double m_fullRange;
  double m_expectedZero;
  units::second_t m_period{0_s};
  double m_sensorMin{0.0};
  double m_sensorMax{1.0};
  bool m_isInverted{false};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
  hal::SimBoolean m_simIsConnected;
};
}  // namespace frc
