/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Encoder.h"

#include <utility>

#include <hal/Encoder.h>
#include <hal/FRCUsageReporting.h>

#include "frc/Base.h"
#include "frc/DigitalInput.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType) {
  m_aSource = std::make_shared<DigitalInput>(aChannel);
  m_bSource = std::make_shared<DigitalInput>(bChannel);
  InitEncoder(reverseDirection, encodingType);
  auto& registry = SendableRegistry::GetInstance();
  registry.AddChild(this, m_aSource.get());
  registry.AddChild(this, m_bSource.get());
}

Encoder::Encoder(DigitalSource* aSource, DigitalSource* bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(aSource, NullDeleter<DigitalSource>()),
      m_bSource(bSource, NullDeleter<DigitalSource>()) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(DigitalSource& aSource, DigitalSource& bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(&aSource, NullDeleter<DigitalSource>()),
      m_bSource(&bSource, NullDeleter<DigitalSource>()) {
  InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource, bool reverseDirection,
                 EncodingType encodingType)
    : m_aSource(aSource), m_bSource(bSource) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

Encoder::~Encoder() {
  int32_t status = 0;
  HAL_FreeEncoder(m_encoder, &status);
  wpi_setHALError(status);
}

int Encoder::Get() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetEncoder(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

void Encoder::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetEncoder(m_encoder, &status);
  wpi_setHALError(status);
}

double Encoder::GetPeriod() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderPeriod(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

void Encoder::SetMaxPeriod(double maxPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod(m_encoder, maxPeriod, &status);
  wpi_setHALError(status);
}

bool Encoder::GetStopped() const {
  if (StatusIsFatal()) return true;
  int32_t status = 0;
  bool value = HAL_GetEncoderStopped(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

bool Encoder::GetDirection() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetEncoderDirection(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

int Encoder::GetRaw() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetEncoderRaw(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

int Encoder::GetEncodingScale() const {
  int32_t status = 0;
  int val = HAL_GetEncoderEncodingScale(m_encoder, &status);
  wpi_setHALError(status);
  return val;
}

double Encoder::GetDistance() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderDistance(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

double Encoder::GetRate() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderRate(m_encoder, &status);
  wpi_setHALError(status);
  return value;
}

void Encoder::SetMinRate(double minRate) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderMinRate(m_encoder, minRate, &status);
  wpi_setHALError(status);
}

void Encoder::SetDistancePerPulse(double distancePerPulse) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse(m_encoder, distancePerPulse, &status);
  wpi_setHALError(status);
}

double Encoder::GetDistancePerPulse() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double distancePerPulse = HAL_GetEncoderDistancePerPulse(m_encoder, &status);
  wpi_setHALError(status);
  return distancePerPulse;
}

void Encoder::SetReverseDirection(bool reverseDirection) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderReverseDirection(m_encoder, reverseDirection, &status);
  wpi_setHALError(status);
}

void Encoder::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Average counter values must be between 1 and 127");
    return;
  }
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage(m_encoder, samplesToAverage, &status);
  wpi_setHALError(status);
}

int Encoder::GetSamplesToAverage() const {
  int32_t status = 0;
  int result = HAL_GetEncoderSamplesToAverage(m_encoder, &status);
  wpi_setHALError(status);
  return result;
}

double Encoder::PIDGet() {
  if (StatusIsFatal()) return 0.0;
  switch (GetPIDSourceType()) {
    case PIDSourceType::kDisplacement:
      return GetDistance();
    case PIDSourceType::kRate:
      return GetRate();
    default:
      return 0.0;
  }
}

void Encoder::SetIndexSource(int channel, Encoder::IndexingType type) {
  // Force digital input if just given an index
  m_indexSource = std::make_shared<DigitalInput>(channel);
  SendableRegistry::GetInstance().AddChild(this, m_indexSource.get());
  SetIndexSource(*m_indexSource.get(), type);
}

void Encoder::SetIndexSource(const DigitalSource& source,
                             Encoder::IndexingType type) {
  int32_t status = 0;
  HAL_SetEncoderIndexSource(
      m_encoder, source.GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)source.GetAnalogTriggerTypeForRouting(),
      (HAL_EncoderIndexingType)type, &status);
  wpi_setHALError(status);
}

void Encoder::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetEncoderSimDevice(m_encoder, device);
}

int Encoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  wpi_setHALError(status);
  return val;
}

void Encoder::InitSendable(SendableBuilder& builder) {
  int32_t status = 0;
  HAL_EncoderEncodingType type = HAL_GetEncoderEncodingType(m_encoder, &status);
  wpi_setHALError(status);
  if (type == HAL_EncoderEncodingType::HAL_Encoder_k4X)
    builder.SetSmartDashboardType("Quadrature Encoder");
  else
    builder.SetSmartDashboardType("Encoder");

  builder.AddDoubleProperty(
      "Speed", [=]() { return GetRate(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance", [=]() { return GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "Distance per Tick", [=]() { return GetDistancePerPulse(); }, nullptr);
}

void Encoder::InitEncoder(bool reverseDirection, EncodingType encodingType) {
  int32_t status = 0;
  m_encoder = HAL_InitializeEncoder(
      m_aSource->GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)m_aSource->GetAnalogTriggerTypeForRouting(),
      m_bSource->GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)m_bSource->GetAnalogTriggerTypeForRouting(),
      reverseDirection, (HAL_EncoderEncodingType)encodingType, &status);
  wpi_setHALError(status);

  HAL_Report(HALUsageReporting::kResourceType_Encoder, GetFPGAIndex() + 1,
             encodingType);
  SendableRegistry::GetInstance().AddLW(this, "Encoder",
                                        m_aSource->GetChannel());
}

double Encoder::DecodingScaleFactor() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double val = HAL_GetEncoderDecodingScaleFactor(m_encoder, &status);
  wpi_setHALError(status);
  return val;
}
