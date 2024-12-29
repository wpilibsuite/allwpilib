// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Counter.h"

#include <memory>

#include <hal/Counter.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/AnalogTrigger.h"
#include "frc/DigitalInput.h"
#include "frc/Errors.h"

using namespace frc;

Counter::Counter(Mode mode) {
  int32_t status = 0;
  m_counter = HAL_InitializeCounter(static_cast<HAL_Counter_Mode>(mode),
                                    &m_index, &status);
  FRC_CheckErrorStatus(status, "InitializeCounter");

  SetMaxPeriod(0.5_s);

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index + 1, mode + 1);
  wpi::SendableRegistry::AddLW(this, "Counter", m_index);
}

Counter::Counter(int channel) : Counter(kTwoPulse) {
  SetUpSource(channel);
  ClearDownSource();
}

Counter::Counter(DigitalSource* source) : Counter(kTwoPulse) {
  SetUpSource(source);
  ClearDownSource();
}

Counter::Counter(std::shared_ptr<DigitalSource> source) : Counter(kTwoPulse) {
  SetUpSource(source);
  ClearDownSource();
}

Counter::Counter(const AnalogTrigger& trigger) : Counter(kTwoPulse) {
  SetUpSource(trigger.CreateOutput(AnalogTriggerType::kState));
  ClearDownSource();
}

Counter::Counter(EncodingType encodingType, DigitalSource* upSource,
                 DigitalSource* downSource, bool inverted)
    : Counter(encodingType,
              std::shared_ptr<DigitalSource>(upSource,
                                             wpi::NullDeleter<DigitalSource>()),
              std::shared_ptr<DigitalSource>(downSource,
                                             wpi::NullDeleter<DigitalSource>()),
              inverted) {}

Counter::Counter(EncodingType encodingType,
                 std::shared_ptr<DigitalSource> upSource,
                 std::shared_ptr<DigitalSource> downSource, bool inverted)
    : Counter(kExternalDirection) {
  if (encodingType != k1X && encodingType != k2X) {
    throw FRC_MakeError(err::ParameterOutOfRange,
                        "Counter only supports 1X and 2X quadrature decoding");
  }
  SetUpSource(upSource);
  SetDownSource(downSource);
  int32_t status = 0;

  if (encodingType == k1X) {
    SetUpSourceEdge(true, false);
    HAL_SetCounterAverageSize(m_counter, 1, &status);
  } else {
    SetUpSourceEdge(true, true);
    HAL_SetCounterAverageSize(m_counter, 2, &status);
  }

  FRC_CheckErrorStatus(status, "Counter constructor");
  SetDownSourceEdge(inverted, true);
}

Counter::~Counter() {
  if (m_counter != HAL_kInvalidHandle) {
    try {
      SetUpdateWhenEmpty(true);
    } catch (const RuntimeError& e) {
      e.Report();
    }
  }
}

void Counter::SetUpSource(int channel) {
  SetUpSource(std::make_shared<DigitalInput>(channel));
  wpi::SendableRegistry::AddChild(this, m_upSource.get());
}

void Counter::SetUpSource(AnalogTrigger* analogTrigger,
                          AnalogTriggerType triggerType) {
  SetUpSource(std::shared_ptr<AnalogTrigger>(analogTrigger,
                                             wpi::NullDeleter<AnalogTrigger>()),
              triggerType);
}

void Counter::SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                          AnalogTriggerType triggerType) {
  SetUpSource(analogTrigger->CreateOutput(triggerType));
}

void Counter::SetUpSource(DigitalSource* source) {
  SetUpSource(std::shared_ptr<DigitalSource>(
      source, wpi::NullDeleter<DigitalSource>()));
}

void Counter::SetUpSource(std::shared_ptr<DigitalSource> source) {
  m_upSource = source;
  int32_t status = 0;
  HAL_SetCounterUpSource(m_counter, source->GetPortHandleForRouting(),
                         static_cast<HAL_AnalogTriggerType>(
                             source->GetAnalogTriggerTypeForRouting()),
                         &status);
  FRC_CheckErrorStatus(status, "SetUpSource");
}

void Counter::SetUpSource(DigitalSource& source) {
  SetUpSource(std::shared_ptr<DigitalSource>(
      &source, wpi::NullDeleter<DigitalSource>()));
}

void Counter::SetUpSourceEdge(bool risingEdge, bool fallingEdge) {
  if (m_upSource == nullptr) {
    throw FRC_MakeError(
        err::NullParameter,
        "Must set non-nullptr UpSource before setting UpSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterUpSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  FRC_CheckErrorStatus(status, "SetUpSourceEdge");
}

void Counter::ClearUpSource() {
  m_upSource.reset();
  int32_t status = 0;
  HAL_ClearCounterUpSource(m_counter, &status);
  FRC_CheckErrorStatus(status, "ClearUpSource");
}

void Counter::SetDownSource(int channel) {
  SetDownSource(std::make_shared<DigitalInput>(channel));
  wpi::SendableRegistry::AddChild(this, m_downSource.get());
}

void Counter::SetDownSource(AnalogTrigger* analogTrigger,
                            AnalogTriggerType triggerType) {
  SetDownSource(std::shared_ptr<AnalogTrigger>(
                    analogTrigger, wpi::NullDeleter<AnalogTrigger>()),
                triggerType);
}

void Counter::SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                            AnalogTriggerType triggerType) {
  SetDownSource(analogTrigger->CreateOutput(triggerType));
}

void Counter::SetDownSource(DigitalSource* source) {
  SetDownSource(std::shared_ptr<DigitalSource>(
      source, wpi::NullDeleter<DigitalSource>()));
}

void Counter::SetDownSource(DigitalSource& source) {
  SetDownSource(std::shared_ptr<DigitalSource>(
      &source, wpi::NullDeleter<DigitalSource>()));
}

void Counter::SetDownSource(std::shared_ptr<DigitalSource> source) {
  m_downSource = source;
  int32_t status = 0;
  HAL_SetCounterDownSource(m_counter, source->GetPortHandleForRouting(),
                           static_cast<HAL_AnalogTriggerType>(
                               source->GetAnalogTriggerTypeForRouting()),
                           &status);
  FRC_CheckErrorStatus(status, "SetDownSource");
}

void Counter::SetDownSourceEdge(bool risingEdge, bool fallingEdge) {
  if (m_downSource == nullptr) {
    throw FRC_MakeError(
        err::NullParameter,
        "Must set non-nullptr DownSource before setting DownSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterDownSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  FRC_CheckErrorStatus(status, "SetDownSourceEdge");
}

void Counter::ClearDownSource() {
  m_downSource.reset();
  int32_t status = 0;
  HAL_ClearCounterDownSource(m_counter, &status);
  FRC_CheckErrorStatus(status, "ClearDownSource");
}

void Counter::SetUpDownCounterMode() {
  int32_t status = 0;
  HAL_SetCounterUpDownMode(m_counter, &status);
  FRC_CheckErrorStatus(status, "SetUpDownCounterMode");
}

void Counter::SetExternalDirectionMode() {
  int32_t status = 0;
  HAL_SetCounterExternalDirectionMode(m_counter, &status);
  FRC_CheckErrorStatus(status, "SetExternalDirectionMode");
}

void Counter::SetSemiPeriodMode(bool highSemiPeriod) {
  int32_t status = 0;
  HAL_SetCounterSemiPeriodMode(m_counter, highSemiPeriod, &status);
  FRC_CheckErrorStatus(status, "SetSemiPeriodMode to {}",
                       highSemiPeriod ? "true" : "false");
}

void Counter::SetPulseLengthMode(double threshold) {
  int32_t status = 0;
  HAL_SetCounterPulseLengthMode(m_counter, threshold, &status);
  FRC_CheckErrorStatus(status, "SetPulseLengthMode");
}

void Counter::SetReverseDirection(bool reverseDirection) {
  int32_t status = 0;
  HAL_SetCounterReverseDirection(m_counter, reverseDirection, &status);
  FRC_CheckErrorStatus(status, "SetReverseDirection to {}",
                       reverseDirection ? "true" : "false");
}

void Counter::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    throw FRC_MakeError(
        err::ParameterOutOfRange,
        "Average counter values must be between 1 and 127, {} out of range",
        samplesToAverage);
  }
  int32_t status = 0;
  HAL_SetCounterSamplesToAverage(m_counter, samplesToAverage, &status);
  FRC_CheckErrorStatus(status, "SetSamplesToAverage to {}", samplesToAverage);
}

int Counter::GetSamplesToAverage() const {
  int32_t status = 0;
  int samples = HAL_GetCounterSamplesToAverage(m_counter, &status);
  FRC_CheckErrorStatus(status, "GetSamplesToAverage");
  return samples;
}

int Counter::GetFPGAIndex() const {
  return m_index;
}

void Counter::SetDistancePerPulse(double distancePerPulse) {
  m_distancePerPulse = distancePerPulse;
}

double Counter::GetDistance() const {
  return Get() * m_distancePerPulse;
}

double Counter::GetRate() const {
  return m_distancePerPulse / GetPeriod().value();
}

int Counter::Get() const {
  int32_t status = 0;
  int value = HAL_GetCounter(m_counter, &status);
  FRC_CheckErrorStatus(status, "Get");
  return value;
}

void Counter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_counter, &status);
  FRC_CheckErrorStatus(status, "Reset");
}

units::second_t Counter::GetPeriod() const {
  int32_t status = 0;
  double value = HAL_GetCounterPeriod(m_counter, &status);
  FRC_CheckErrorStatus(status, "GetPeriod");
  return units::second_t{value};
}

void Counter::SetMaxPeriod(units::second_t maxPeriod) {
  int32_t status = 0;
  HAL_SetCounterMaxPeriod(m_counter, maxPeriod.value(), &status);
  FRC_CheckErrorStatus(status, "SetMaxPeriod");
}

void Counter::SetUpdateWhenEmpty(bool enabled) {
  int32_t status = 0;
  HAL_SetCounterUpdateWhenEmpty(m_counter, enabled, &status);
  FRC_CheckErrorStatus(status, "SetUpdateWhenEmpty");
}

bool Counter::GetStopped() const {
  int32_t status = 0;
  bool value = HAL_GetCounterStopped(m_counter, &status);
  FRC_CheckErrorStatus(status, "GetStopped");
  return value;
}

bool Counter::GetDirection() const {
  int32_t status = 0;
  bool value = HAL_GetCounterDirection(m_counter, &status);
  FRC_CheckErrorStatus(status, "GetDirection");
  return value;
}

void Counter::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Counter");
  builder.AddDoubleProperty("Value", [=, this] { return Get(); }, nullptr);
}
