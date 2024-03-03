// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {
class AnalogInput;
class Counter;
class AnalogTrigger;

/**
 * Class for supporting continuous analog encoders, such as the US Digital MA3.
 */
class AnalogEncoder : public wpi::Sendable,
                      public wpi::SendableHelper<AnalogEncoder> {
 public:
  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   */
  explicit AnalogEncoder(int channel);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput& analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput* analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(std::shared_ptr<AnalogInput> analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   */
  AnalogEncoder(int channel, double fullRange, double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  AnalogEncoder(AnalogInput& analogInput, double fullRange,
                double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  AnalogEncoder(AnalogInput* analogInput, double fullRange,
                double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  AnalogEncoder(std::shared_ptr<AnalogInput> analogInput, double fullRange,
                double expectedZero);

  ~AnalogEncoder() override;

  AnalogEncoder(AnalogEncoder&&) = default;
  AnalogEncoder& operator=(AnalogEncoder&&) = default;

  /**
   * Get the encoder value.
   *
   * @return the encoder value scaled by the full range input
   */
  double Get() const;

  /**
   * Set the encoder voltage percentage range. Analog sensors are not always
   * fully stable at the end of their travel ranges. Shrinking this range down
   * can help mitigate issues with that.
   *
   * @param min minimum voltage percentage (0-1 range)
   * @param max maximum voltage percentage (0-1 range)
   */
  void SetVoltagePercentageRange(double min, double max);

  /**
   * Set if this encoder is inverted.
   *
   * @param inverted true to invert the encoder, false otherwise
   */
  void SetInverted(bool inverted);

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

  /**
   * Configures if this encoder has rollover counting enabled.
   *
   * <p> By default, the encoder will not count rollovers. This
   * behavior is very rarely needed, and is usually a sign you are
   * using the wrong encoder type.
   *
   * @param enable True to enable rollover counting, false to disable.
   */
  void ConfigureRolloverSupport(bool enable);

  /**
   * Reset the number of rollovers that have been counted.
   *
   * <p>This has no effect unless configureRolloverSupport(true)
   * has been called.
   */
  void ResetRollovers();

 private:
  void Init(double fullRange, double expectedZero);
  double MapSensorRange(double pos) const;

  double GetWithoutRollovers(double analog) const;
  double GetWithRollovers() const;

  std::shared_ptr<AnalogInput> m_analogInput;
  double m_fullRange;
  double m_expectedZero;
  double m_sensorMin{0.0};
  double m_sensorMax{1.0};
  bool m_isInverted{false};

  std::unique_ptr<AnalogTrigger> m_rolloverTrigger{nullptr};
  std::unique_ptr<Counter> m_rolloverCounter{nullptr};
  mutable double m_lastPosition{0.0};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
};
}  // namespace frc
