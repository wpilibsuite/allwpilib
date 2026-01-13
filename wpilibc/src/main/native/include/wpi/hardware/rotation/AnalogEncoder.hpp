// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/hal/SimDevice.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"

namespace wpi {
class AnalogInput;

/**
 * Class for supporting continuous analog encoders, such as the US Digital MA3.
 */
class AnalogEncoder : public wpi::TelemetryLoggable {
 public:
  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param channel the analog input channel to attach to
   */
  explicit AnalogEncoder(int channel);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput& analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput* analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * <p>This has a fullRange of 1 and an expectedZero of 0.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(std::shared_ptr<AnalogInput> analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogIn channel.
   *
   * @param channel the analog input channel to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  AnalogEncoder(int channel, double fullRange, double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  AnalogEncoder(AnalogInput& analogInput, double fullRange,
                double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
   */
  AnalogEncoder(AnalogInput* analogInput, double fullRange,
                double expectedZero);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   * @param fullRange the value to report at maximum travel
   * @param expectedZero the reading where you would expect a 0 from get()
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

  void LogTo(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  void Init(double fullRange, double expectedZero);
  double MapSensorRange(double pos) const;

  std::shared_ptr<AnalogInput> m_analogInput;
  double m_fullRange;
  double m_expectedZero;
  double m_sensorMin{0.0};
  double m_sensorMax{1.0};
  bool m_isInverted{false};

  wpi::hal::SimDevice m_simDevice;
  wpi::hal::SimDouble m_simPosition;
};
}  // namespace wpi
