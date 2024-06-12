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
class AnalogInput;

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

  ~AnalogEncoder() override = default;

  AnalogEncoder(AnalogEncoder&&) = default;
  AnalogEncoder& operator=(AnalogEncoder&&) = default;

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
   * Get the absolute position of the analog encoder.
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
   * Get the channel number.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  void Init();

  std::shared_ptr<AnalogInput> m_analogInput;
  AnalogTrigger m_analogTrigger;
  Counter m_counter;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
  mutable units::turn_t m_lastPosition{0.0};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
  hal::SimDouble m_simAbsolutePosition;
};
}  // namespace frc
