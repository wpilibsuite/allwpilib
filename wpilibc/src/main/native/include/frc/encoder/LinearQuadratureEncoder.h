// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <units/length.h>
#include <units/velocity.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/DigitalSource.h"
#include "frc/encoder/LinearEncoder.h"

namespace wpi {
class SendableBuilder;
}  // namespace wpi

namespace frc {

class LinearQuadratureEncoder
    : public LinearEncoder,
      public wpi::Sendable,
      public wpi::SendableHelper<LinearQuadratureEncoder> {
 public:
  enum class EncodingType { k1X, k2X, k4X };

  LinearQuadratureEncoder(int aChannel, int bChannel,
                          decltype(1.0 / 1_m) cyclesPerMeter,
                          EncodingType encodingType = EncodingType::k4X);
  LinearQuadratureEncoder(DigitalSource& aSource, DigitalSource& bSource,
                          decltype(1.0 / 1_m) cyclesPerMeter,
                          EncodingType encodingType = EncodingType::k4X);
  LinearQuadratureEncoder(std::shared_ptr<DigitalSource> aSource,
                          std::shared_ptr<DigitalSource> bSource,
                          decltype(1.0 / 1_m) cyclesPerMeter,
                          EncodingType encodingType = EncodingType::k4X);

  LinearQuadratureEncoder(LinearQuadratureEncoder&&) = default;
  LinearQuadratureEncoder& operator=(LinearQuadratureEncoder&&) = default;

  units::meter_t GetDisplacement() const override;

  units::meters_per_second_t GetVelocity() const override;

  void SetInverted(bool inverted) override;

  void Reset() override;

  void SetSimDevice(HAL_SimDeviceHandle device);

  int GetFPGAIndex() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::shared_ptr<DigitalSource> m_aSource;  // The A phase of the quad encoder
  std::shared_ptr<DigitalSource> m_bSource;  // The B phase of the quad encoder
  hal::Handle<HAL_EncoderHandle> m_encoder;

  void Init(decltype(1.0 / 1_m) cyclesPerMeter, EncodingType encodingType);
};

}  // namespace frc
