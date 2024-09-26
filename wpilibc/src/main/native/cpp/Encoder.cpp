// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Encoder.h"

#include <memory>
#include <utility>

#include <hal/Encoder.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/DigitalInput.h"
#include "frc/Errors.h"

using namespace frc;

Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType) {
  m_aSource = std::make_shared<DigitalInput>(aChannel);
  m_bSource = std::make_shared<DigitalInput>(bChannel);
  InitEncoder(reverseDirection, encodingType);
  wpi::SendableRegistry::AddChild(this, m_aSource.get());
  wpi::SendableRegistry::AddChild(this, m_bSource.get());
}

Encoder::Encoder(DigitalSource* aSource, DigitalSource* bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(aSource, wpi::NullDeleter<DigitalSource>()),
      m_bSource(bSource, wpi::NullDeleter<DigitalSource>()) {
  if (!m_aSource) {
    throw FRC_MakeError(err::NullParameter, "aSource");
  }
  if (!m_bSource) {
    throw FRC_MakeError(err::NullParameter, "bSource");
  }
  InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(DigitalSource& aSource, DigitalSource& bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(&aSource, wpi::NullDeleter<DigitalSource>()),
      m_bSource(&bSource, wpi::NullDeleter<DigitalSource>()) {
  InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource, bool reverseDirection,
                 EncodingType encodingType)
    : m_aSource(std::move(aSource)), m_bSource(std::move(bSource)) {
  if (!m_aSource) {
    throw FRC_MakeError(err::NullParameter, "aSource");
  }
  if (!m_bSource) {
    throw FRC_MakeError(err::NullParameter, "bSource");
  }
  InitEncoder(reverseDirection, encodingType);
}

int Encoder::Get() const {
  int32_t status = 0;
  int value = HAL_GetEncoder(m_encoder, &status);
  FRC_CheckErrorStatus(status, "Get");
  return value;
}

void Encoder::Reset() {
  int32_t status = 0;
  HAL_ResetEncoder(m_encoder, &status);
  FRC_CheckErrorStatus(status, "Reset");
}

units::second_t Encoder::GetPeriod() const {
  int32_t status = 0;
  double value = HAL_GetEncoderPeriod(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetPeriod");
  return units::second_t{value};
}

void Encoder::SetMaxPeriod(units::second_t maxPeriod) {
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod(m_encoder, maxPeriod.value(), &status);
  FRC_CheckErrorStatus(status, "SetMaxPeriod");
}

bool Encoder::GetStopped() const {
  int32_t status = 0;
  bool value = HAL_GetEncoderStopped(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetStopped");
  return value;
}

bool Encoder::GetDirection() const {
  int32_t status = 0;
  bool value = HAL_GetEncoderDirection(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetDirection");
  return value;
}

int Encoder::GetRaw() const {
  int32_t status = 0;
  int value = HAL_GetEncoderRaw(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetRaw");
  return value;
}

int Encoder::GetEncodingScale() const {
  int32_t status = 0;
  int val = HAL_GetEncoderEncodingScale(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetEncodingScale");
  return val;
}

double Encoder::GetDistance() const {
  int32_t status = 0;
  double value = HAL_GetEncoderDistance(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetDistance");
  return value;
}

double Encoder::GetRate() const {
  int32_t status = 0;
  double value = HAL_GetEncoderRate(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetRate");
  return value;
}

void Encoder::SetMinRate(double minRate) {
  int32_t status = 0;
  HAL_SetEncoderMinRate(m_encoder, minRate, &status);
  FRC_CheckErrorStatus(status, "SetMinRate");
}

void Encoder::SetDistancePerPulse(double distancePerPulse) {
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse(m_encoder, distancePerPulse, &status);
  FRC_CheckErrorStatus(status, "SetDistancePerPulse");
}

double Encoder::GetDistancePerPulse() const {
  int32_t status = 0;
  double distancePerPulse = HAL_GetEncoderDistancePerPulse(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetDistancePerPulse");
  return distancePerPulse;
}

void Encoder::SetReverseDirection(bool reverseDirection) {
  int32_t status = 0;
  HAL_SetEncoderReverseDirection(m_encoder, reverseDirection, &status);
  FRC_CheckErrorStatus(status, "SetReverseDirection");
}

void Encoder::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    throw FRC_MakeError(
        err::ParameterOutOfRange,
        "Average counter values must be between 1 and 127, got {}",
        samplesToAverage);
  }
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage(m_encoder, samplesToAverage, &status);
  FRC_CheckErrorStatus(status, "SetSamplesToAverage");
}

int Encoder::GetSamplesToAverage() const {
  int32_t status = 0;
  int result = HAL_GetEncoderSamplesToAverage(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetSamplesToAverage");
  return result;
}

void Encoder::SetIndexSource(int channel, Encoder::IndexingType type) {
  // Force digital input if just given an index
  m_indexSource = std::make_shared<DigitalInput>(channel);
  wpi::SendableRegistry::AddChild(this, m_indexSource.get());
  SetIndexSource(*m_indexSource.get(), type);
}

void Encoder::SetIndexSource(const DigitalSource& source,
                             Encoder::IndexingType type) {
  int32_t status = 0;
  HAL_SetEncoderIndexSource(m_encoder, source.GetPortHandleForRouting(),
                            static_cast<HAL_AnalogTriggerType>(
                                source.GetAnalogTriggerTypeForRouting()),
                            static_cast<HAL_EncoderIndexingType>(type),
                            &status);
  FRC_CheckErrorStatus(status, "SetIndexSource");
}

void Encoder::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetEncoderSimDevice(m_encoder, device);
}

int Encoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetFPGAIndex");
  return val;
}

void Encoder::InitSendable(wpi::SendableBuilder& builder) {
  int32_t status = 0;
  HAL_EncoderEncodingType type = HAL_GetEncoderEncodingType(m_encoder, &status);
  FRC_CheckErrorStatus(status, "GetEncodingType");
  if (type == HAL_EncoderEncodingType::HAL_Encoder_k4X) {
    builder.SetSmartDashboardType("Quadrature Encoder");
  } else {
    builder.SetSmartDashboardType("Encoder");
  }

  builder.AddDoubleProperty("Speed", [=, this] { return GetRate(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance", [=, this] { return GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance per Tick", [=, this] { return GetDistancePerPulse(); },
      nullptr);
}

void Encoder::InitEncoder(bool reverseDirection, EncodingType encodingType) {
  int32_t status = 0;
  m_encoder = HAL_InitializeEncoder(
      m_aSource->GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(
          m_aSource->GetAnalogTriggerTypeForRouting()),
      m_bSource->GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(
          m_bSource->GetAnalogTriggerTypeForRouting()),
      reverseDirection, static_cast<HAL_EncoderEncodingType>(encodingType),
      &status);
  FRC_CheckErrorStatus(status, "InitEncoder");

  HAL_Report(HALUsageReporting::kResourceType_Encoder, GetFPGAIndex() + 1,
             encodingType);
  wpi::SendableRegistry::AddLW(this, "Encoder", m_aSource->GetChannel());
}

double Encoder::DecodingScaleFactor() const {
  int32_t status = 0;
  double val = HAL_GetEncoderDecodingScaleFactor(m_encoder, &status);
  FRC_CheckErrorStatus(status, "DecodingScaleFactor");
  return val;
}
