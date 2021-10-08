// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/encoder/RotaryQuadratureEncoder.h"

#include <hal/Encoder.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/numbers>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/DigitalInput.h"
#include "frc/Errors.h"

using namespace frc;

RotaryQuadratureEncoder::RotaryQuadratureEncoder(
    int aChannel, int bChannel, decltype(1.0 / 1_tr) cyclesPerRevolution,
    EncodingType encodingType)
    : m_aSource{std::make_shared<DigitalInput>(aChannel)},
      m_bSource{std::make_shared<DigitalInput>(bChannel)} {
  Init(cyclesPerRevolution, encodingType);
  wpi::SendableRegistry::AddChild(this, m_aSource.get());
  wpi::SendableRegistry::AddChild(this, m_bSource.get());
}

RotaryQuadratureEncoder::RotaryQuadratureEncoder(
    DigitalSource& aSource, DigitalSource& bSource,
    decltype(1.0 / 1_tr) cyclesPerRevolution, EncodingType encodingType)
    : m_aSource(&aSource, wpi::NullDeleter<DigitalSource>()),
      m_bSource(&bSource, wpi::NullDeleter<DigitalSource>()) {
  Init(cyclesPerRevolution, encodingType);
}

RotaryQuadratureEncoder::RotaryQuadratureEncoder(
    std::shared_ptr<DigitalSource> aSource,
    std::shared_ptr<DigitalSource> bSource,
    decltype(1.0 / 1_tr) cyclesPerRevolution, EncodingType encodingType)
    : m_aSource{std::move(aSource)}, m_bSource{std::move(bSource)} {
  Init(cyclesPerRevolution, encodingType);
}

units::radian_t RotaryQuadratureEncoder::GetAngle() const {
  int32_t status = 0;
  double value = HAL_GetEncoderDistance(m_encoder, &status);
  FRC_CheckErrorStatus(status, "{}", "GetDisplacement");
  return units::radian_t{value};
}

units::radians_per_second_t RotaryQuadratureEncoder::GetAngularVelocity()
    const {
  int32_t status = 0;
  double value = HAL_GetEncoderRate(m_encoder, &status);
  FRC_CheckErrorStatus(status, "{}", "GetVelocity");
  return units::radians_per_second_t{value};
}

void RotaryQuadratureEncoder::SetInverted(bool inverted) {
  int32_t status = 0;
  HAL_SetEncoderReverseDirection(m_encoder, inverted, &status);
  FRC_CheckErrorStatus(status, "{}", "SetInverted");
}

void RotaryQuadratureEncoder::Reset() {
  int32_t status = 0;
  HAL_ResetEncoder(m_encoder, &status);
  FRC_CheckErrorStatus(status, "{}", "Reset");
}

void RotaryQuadratureEncoder::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetEncoderSimDevice(m_encoder, device);
}

int RotaryQuadratureEncoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  FRC_CheckErrorStatus(status, "{}", "GetFPGAIndex");
  return val;
}

void RotaryQuadratureEncoder::Init(decltype(1.0 / 1_tr) cyclesPerRevolution,
                                   EncodingType encodingType) {
  int32_t status = 0;
  m_encoder = HAL_InitializeEncoder(
      m_aSource->GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(
          m_aSource->GetAnalogTriggerTypeForRouting()),
      m_bSource->GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(
          m_bSource->GetAnalogTriggerTypeForRouting()),
      false, static_cast<HAL_EncoderEncodingType>(encodingType), &status);
  FRC_CheckErrorStatus(status, "{}", "Init");

  HAL_Report(HALUsageReporting::kResourceType_Encoder, GetFPGAIndex() + 1,
             static_cast<int32_t>(encodingType));
  wpi::SendableRegistry::AddLW(this, "RotaryQuadratureEncoder",
                               m_aSource->GetChannel());

  status = 0;
  HAL_SetEncoderDistancePerPulse(
      m_encoder, 2.0 * wpi::numbers::pi / cyclesPerRevolution.to<double>(),
      &status);
  FRC_CheckErrorStatus(status, "{}", "SetDistancePerPulse");
}

void RotaryQuadratureEncoder::InitSendable(wpi::SendableBuilder& builder) {
  builder.AddDoubleProperty(
      "Angle", [=] { return GetAngle().to<double>(); }, nullptr);
  builder.AddDoubleProperty(
      "AngularVelocity", [=] { return GetAngularVelocity().to<double>(); },
      nullptr);
}
