/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Counter.h"

#include <utility>

#include <hal/Counter.h>
#include <hal/FRCUsageReporting.h>

#include "frc/AnalogTrigger.h"
#include "frc/Base.h"
#include "frc/DigitalInput.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Counter::Counter(Mode mode) {
  int32_t status = 0;
  m_counter = HAL_InitializeCounter((HAL_Counter_Mode)mode, &m_index, &status);
  wpi_setHALError(status);

  SetMaxPeriod(0.5);

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index + 1, mode + 1);
  SendableRegistry::GetInstance().AddLW(this, "Counter", m_index);
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
                                             NullDeleter<DigitalSource>()),
              std::shared_ptr<DigitalSource>(downSource,
                                             NullDeleter<DigitalSource>()),
              inverted) {}

Counter::Counter(EncodingType encodingType,
                 std::shared_ptr<DigitalSource> upSource,
                 std::shared_ptr<DigitalSource> downSource, bool inverted)
    : Counter(kExternalDirection) {
  if (encodingType != k1X && encodingType != k2X) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Counter only supports 1X and 2X quadrature decoding.");
    return;
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

  wpi_setHALError(status);
  SetDownSourceEdge(inverted, true);
}

Counter::~Counter() {
  SetUpdateWhenEmpty(true);

  int32_t status = 0;
  HAL_FreeCounter(m_counter, &status);
  wpi_setHALError(status);
}

void Counter::SetUpSource(int channel) {
  if (StatusIsFatal()) return;
  SetUpSource(std::make_shared<DigitalInput>(channel));
  SendableRegistry::GetInstance().AddChild(this, m_upSource.get());
}

void Counter::SetUpSource(AnalogTrigger* analogTrigger,
                          AnalogTriggerType triggerType) {
  SetUpSource(std::shared_ptr<AnalogTrigger>(analogTrigger,
                                             NullDeleter<AnalogTrigger>()),
              triggerType);
}

void Counter::SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                          AnalogTriggerType triggerType) {
  if (StatusIsFatal()) return;
  SetUpSource(analogTrigger->CreateOutput(triggerType));
}

void Counter::SetUpSource(DigitalSource* source) {
  SetUpSource(
      std::shared_ptr<DigitalSource>(source, NullDeleter<DigitalSource>()));
}

void Counter::SetUpSource(std::shared_ptr<DigitalSource> source) {
  if (StatusIsFatal()) return;
  m_upSource = source;
  if (m_upSource->StatusIsFatal()) {
    CloneError(*m_upSource);
  } else {
    int32_t status = 0;
    HAL_SetCounterUpSource(
        m_counter, source->GetPortHandleForRouting(),
        (HAL_AnalogTriggerType)source->GetAnalogTriggerTypeForRouting(),
        &status);
    wpi_setHALError(status);
  }
}

void Counter::SetUpSource(DigitalSource& source) {
  SetUpSource(
      std::shared_ptr<DigitalSource>(&source, NullDeleter<DigitalSource>()));
}

void Counter::SetUpSourceEdge(bool risingEdge, bool fallingEdge) {
  if (StatusIsFatal()) return;
  if (m_upSource == nullptr) {
    wpi_setWPIErrorWithContext(
        NullParameter,
        "Must set non-nullptr UpSource before setting UpSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterUpSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  wpi_setHALError(status);
}

void Counter::ClearUpSource() {
  if (StatusIsFatal()) return;
  m_upSource.reset();
  int32_t status = 0;
  HAL_ClearCounterUpSource(m_counter, &status);
  wpi_setHALError(status);
}

void Counter::SetDownSource(int channel) {
  if (StatusIsFatal()) return;
  SetDownSource(std::make_shared<DigitalInput>(channel));
  SendableRegistry::GetInstance().AddChild(this, m_downSource.get());
}

void Counter::SetDownSource(AnalogTrigger* analogTrigger,
                            AnalogTriggerType triggerType) {
  SetDownSource(std::shared_ptr<AnalogTrigger>(analogTrigger,
                                               NullDeleter<AnalogTrigger>()),
                triggerType);
}

void Counter::SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                            AnalogTriggerType triggerType) {
  if (StatusIsFatal()) return;
  SetDownSource(analogTrigger->CreateOutput(triggerType));
}

void Counter::SetDownSource(DigitalSource* source) {
  SetDownSource(
      std::shared_ptr<DigitalSource>(source, NullDeleter<DigitalSource>()));
}

void Counter::SetDownSource(DigitalSource& source) {
  SetDownSource(
      std::shared_ptr<DigitalSource>(&source, NullDeleter<DigitalSource>()));
}

void Counter::SetDownSource(std::shared_ptr<DigitalSource> source) {
  if (StatusIsFatal()) return;
  m_downSource = source;
  if (m_downSource->StatusIsFatal()) {
    CloneError(*m_downSource);
  } else {
    int32_t status = 0;
    HAL_SetCounterDownSource(
        m_counter, source->GetPortHandleForRouting(),
        (HAL_AnalogTriggerType)source->GetAnalogTriggerTypeForRouting(),
        &status);
    wpi_setHALError(status);
  }
}

void Counter::SetDownSourceEdge(bool risingEdge, bool fallingEdge) {
  if (StatusIsFatal()) return;
  if (m_downSource == nullptr) {
    wpi_setWPIErrorWithContext(
        NullParameter,
        "Must set non-nullptr DownSource before setting DownSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterDownSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  wpi_setHALError(status);
}

void Counter::ClearDownSource() {
  if (StatusIsFatal()) return;
  m_downSource.reset();
  int32_t status = 0;
  HAL_ClearCounterDownSource(m_counter, &status);
  wpi_setHALError(status);
}

void Counter::SetUpDownCounterMode() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterUpDownMode(m_counter, &status);
  wpi_setHALError(status);
}

void Counter::SetExternalDirectionMode() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterExternalDirectionMode(m_counter, &status);
  wpi_setHALError(status);
}

void Counter::SetSemiPeriodMode(bool highSemiPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterSemiPeriodMode(m_counter, highSemiPeriod, &status);
  wpi_setHALError(status);
}

void Counter::SetPulseLengthMode(double threshold) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterPulseLengthMode(m_counter, threshold, &status);
  wpi_setHALError(status);
}

void Counter::SetReverseDirection(bool reverseDirection) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterReverseDirection(m_counter, reverseDirection, &status);
  wpi_setHALError(status);
}

void Counter::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Average counter values must be between 1 and 127");
  }
  int32_t status = 0;
  HAL_SetCounterSamplesToAverage(m_counter, samplesToAverage, &status);
  wpi_setHALError(status);
}

int Counter::GetSamplesToAverage() const {
  int32_t status = 0;
  int samples = HAL_GetCounterSamplesToAverage(m_counter, &status);
  wpi_setHALError(status);
  return samples;
}

int Counter::GetFPGAIndex() const { return m_index; }

int Counter::Get() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetCounter(m_counter, &status);
  wpi_setHALError(status);
  return value;
}

void Counter::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetCounter(m_counter, &status);
  wpi_setHALError(status);
}

double Counter::GetPeriod() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetCounterPeriod(m_counter, &status);
  wpi_setHALError(status);
  return value;
}

void Counter::SetMaxPeriod(double maxPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterMaxPeriod(m_counter, maxPeriod, &status);
  wpi_setHALError(status);
}

void Counter::SetUpdateWhenEmpty(bool enabled) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterUpdateWhenEmpty(m_counter, enabled, &status);
  wpi_setHALError(status);
}

bool Counter::GetStopped() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetCounterStopped(m_counter, &status);
  wpi_setHALError(status);
  return value;
}

bool Counter::GetDirection() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetCounterDirection(m_counter, &status);
  wpi_setHALError(status);
  return value;
}

void Counter::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Counter");
  builder.AddDoubleProperty(
      "Value", [=]() { return Get(); }, nullptr);
}
