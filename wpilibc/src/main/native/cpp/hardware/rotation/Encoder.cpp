// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/Encoder.hpp"

#include <format>

#include "wpi/hal/Encoder.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

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

void Encoder::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetEncoderSimDevice(m_encoder, device);
}

int Encoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "GetFPGAIndex");
  return val;
}

void Encoder::LogTo(wpi::TelemetryTable& table) const {
  table.Log("Velocity", GetRate());
  table.Log("Distance", GetDistance());
  table.Log("Distance per Tick", GetDistancePerPulse());
}

std::string_view Encoder::GetTelemetryType() const {
  if (m_type == EncodingType::X4) {
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
    case EncodingType::X1:
      type = "Encoder:1x";
      break;
    case EncodingType::X2:
      type = "Encoder:2x";
      break;
    case EncodingType::X4:
      type = "Encoder:4x";
      break;
  }
  HAL_ReportUsage(std::format("IO[{},{}]", aChannel, bChannel), type);
}

double Encoder::DecodingScaleFactor() const {
  int32_t status = 0;
  double val = HAL_GetEncoderDecodingScaleFactor(m_encoder, &status);
  WPILIB_CheckErrorStatus(status, "DecodingScaleFactor");
  return val;
}
