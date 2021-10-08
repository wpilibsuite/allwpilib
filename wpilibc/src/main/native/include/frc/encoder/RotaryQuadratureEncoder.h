// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/DigitalSource.h"
#include "frc/encoder/RotaryEncoder.h"

namespace wpi {
class SendableBuilder;
}  // namespace wpi

namespace frc {

class RotaryQuadratureEncoder
    : public RotaryEncoder,
      public wpi::Sendable,
      public wpi::SendableHelper<RotaryQuadratureEncoder> {
 public:
  enum class EncodingType { k1X, k2X, k4X };

  RotaryQuadratureEncoder(int aChannel, int bChannel,
                          decltype(1.0 / 1_tr) cyclesPerRevolution,
                          EncodingType encodingType = EncodingType::k4X);
  RotaryQuadratureEncoder(DigitalSource& aSource, DigitalSource& bSource,
                          decltype(1.0 / 1_tr) cyclesPerRevolution,
                          EncodingType encodingType = EncodingType::k4X);
  RotaryQuadratureEncoder(std::shared_ptr<DigitalSource> aSource,
                          std::shared_ptr<DigitalSource> bSource,
                          decltype(1.0 / 1_tr) cyclesPerRevolution,
                          EncodingType encodingType = EncodingType::k4X);

  RotaryQuadratureEncoder(RotaryQuadratureEncoder&&) = default;
  RotaryQuadratureEncoder& operator=(RotaryQuadratureEncoder&&) = default;

  units::radian_t GetAngle() const override;

  units::radians_per_second_t GetAngularVelocity() const override;

  void SetInverted(bool inverted) override;

  void Reset() override;

  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetFPGAIndex() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<DigitalSource> m_aSource;  // The A phase of the quad encoder
  std::shared_ptr<DigitalSource> m_bSource;  // The B phase of the quad encoder
  hal::Handle<HAL_EncoderHandle> m_encoder;

  void Init(decltype(1.0 / 1_tr) cyclesPerRevolution,
            EncodingType encodingType);
};

}  // namespace frc
