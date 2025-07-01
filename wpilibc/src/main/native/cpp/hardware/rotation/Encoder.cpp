// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/Encoder.hpp"

#include <memory>
#include <utility>

#include "wpi/hal/Encoder.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/NullDeleter.hpp"

using namespace wpi;

Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType) {
  InitEncoder(aChannel, bChannel, reverseDirection, encodingType);
}

int Encoder::Get() const {
  int32_t status = 0;
  int value = HAL_GetEncoder(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "Get");
  return value;
}

void Encoder::Reset() {
  int32_t status = 0;
  HAL_ResetEncoder(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "Reset");
}

wpi::units::second_t Encoder::GetPeriod() const {
  int32_t status = 0;
  double value = HAL_GetEncoderPeriod(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetPeriod");
  return wpi::units::second_t{value};
}

void Encoder::SetMaxPeriod(wpi::units::second_t maxPeriod) {
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod(m_encoder, maxPeriod.value(), &status);
  WPILIB_CheckErrorStatus(status, "SetMaxPeriod");
}

bool Encoder::GetStopped() const {
  int32_t status = 0;
  bool value = HAL_GetEncoderStopped(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetStopped");
  return value;
}

bool Encoder::GetDirection() const {
  int32_t status = 0;
  bool value = HAL_GetEncoderDirection(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetDirection");
  return value;
}

int Encoder::GetRaw() const {
  int32_t status = 0;
  int value = HAL_GetEncoderRaw(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetRaw");
  return value;
}

int Encoder::GetEncodingScale() const {
  int32_t status = 0;
  int val = HAL_GetEncoderEncodingScale(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetEncodingScale");
  return val;
}

double Encoder::GetDistance() const {
  int32_t status = 0;
  double value = HAL_GetEncoderDistance(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetDistance");
  return value;
}

double Encoder::GetRate() const {
  int32_t status = 0;
  double value = HAL_GetEncoderRate(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetRate");
  return value;
}

void Encoder::SetMinRate(double minRate) {
  int32_t status = 0;
  HAL_SetEncoderMinRate(m_encoder, minRate, &status);
  WPILIB_CheckErrorStatus(status, "SetMinRate");
}

void Encoder::SetDistancePerPulse(double distancePerPulse) {
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse(m_encoder, distancePerPulse, &status);
  WPILIB_CheckErrorStatus(status, "SetDistancePerPulse");
}

double Encoder::GetDistancePerPulse() const {
  int32_t status = 0;
  double distancePerPulse = HAL_GetEncoderDistancePerPulse(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetDistancePerPulse");
  return distancePerPulse;
}

void Encoder::SetReverseDirection(bool reverseDirection) {
  int32_t status = 0;
  HAL_SetEncoderReverseDirection(m_encoder, reverseDirection, &status);
  WPILIB_CheckErrorStatus(status, "SetReverseDirection");
}

void Encoder::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    throw WPILIB_MakeError(
        err::ParameterOutOfRange,
        "Average counter values must be between 1 and 127, got {}",
        samplesToAverage);
  }
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage(m_encoder, samplesToAverage, &status);
  WPILIB_CheckErrorStatus(status, "SetSamplesToAverage");
}

int Encoder::GetSamplesToAverage() const {
  int32_t status = 0;
  int result = HAL_GetEncoderSamplesToAverage(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetSamplesToAverage");
  return result;
}

void Encoder::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetEncoderSimDevice(m_encoder, device);
}

int Encoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetFPGAIndex");
  return val;
}

void Encoder::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Speed", GetRate());
  table.Log("Distance", GetDistance());
  table.Log("Distance per Tick", GetDistancePerPulse());
}

std::string_view Encoder::GetTelemetryType() const {
  if (m_type == EncodingType::k4X) {
    return "Quadrature Encoder";
  } else {
    return "Encoder";
  }
}

void Encoder::InitEncoder(int aChannel, int bChannel, bool reverseDirection,
                          EncodingType encodingType) {
  int32_t status = 0;
  m_encoder = HAL_InitializeEncoder(
      aChannel, bChannel, reverseDirection,
      static_cast<HAL_EncoderEncodingType>(encodingType), &status);
  m_type = encodingType;
  WPILIB_CheckErrorStatus(status, "InitEncoder");

  const char* type = "Encoder";
  switch (encodingType) {
    case k1X:
      type = "Encoder:1x";
      break;
    case k2X:
      type = "Encoder:2x";
      break;
    case k4X:
      type = "Encoder:4x";
      break;
  }
  HAL_ReportUsage(fmt::format("IO[{},{}]", aChannel, bChannel), type);
}

double Encoder::DecodingScaleFactor() const {
  int32_t status = 0;
  double val = HAL_GetEncoderDecodingScaleFactor(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "DecodingScaleFactor");
  return val;
}
