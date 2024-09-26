// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DMA.h"

#include <frc/AnalogInput.h>
#include <frc/Counter.h>
#include <frc/DigitalSource.h>
#include <frc/DutyCycle.h>
#include <frc/Encoder.h>
#include <frc/PWM.h>
#include <frc/motorcontrol/PWMMotorController.h>

#include <hal/DMA.h>
#include <hal/HALBase.h>

#include "frc/Errors.h"

using namespace frc;

DMA::DMA() {
  int32_t status = 0;
  dmaHandle = HAL_InitializeDMA(&status);
  FRC_CheckErrorStatus(status, "InitializeDMA");
}

void DMA::SetPause(bool pause) {
  int32_t status = 0;
  HAL_SetDMAPause(dmaHandle, pause, &status);
  FRC_CheckErrorStatus(status, "SetPause");
}

void DMA::SetTimedTrigger(units::second_t period) {
  int32_t status = 0;
  HAL_SetDMATimedTrigger(dmaHandle, period.value(), &status);
  FRC_CheckErrorStatus(status, "SetTimedTrigger");
}

void DMA::SetTimedTriggerCycles(int cycles) {
  int32_t status = 0;
  HAL_SetDMATimedTriggerCycles(dmaHandle, cycles, &status);
  FRC_CheckErrorStatus(status, "SetTimedTriggerCycles");
}

void DMA::AddEncoder(const Encoder* encoder) {
  int32_t status = 0;
  HAL_AddDMAEncoder(dmaHandle, encoder->m_encoder, &status);
  FRC_CheckErrorStatus(status, "AddEncoder");
}

void DMA::AddEncoderPeriod(const Encoder* encoder) {
  int32_t status = 0;
  HAL_AddDMAEncoderPeriod(dmaHandle, encoder->m_encoder, &status);
  FRC_CheckErrorStatus(status, "AddEncoderPeriod");
}

void DMA::AddCounter(const Counter* counter) {
  int32_t status = 0;
  HAL_AddDMACounter(dmaHandle, counter->m_counter, &status);
  FRC_CheckErrorStatus(status, "AddCounter");
}

void DMA::AddCounterPeriod(const Counter* counter) {
  int32_t status = 0;
  HAL_AddDMACounterPeriod(dmaHandle, counter->m_counter, &status);
  FRC_CheckErrorStatus(status, "AddCounterPeriod");
}

void DMA::AddDigitalSource(const DigitalSource* digitalSource) {
  int32_t status = 0;
  HAL_AddDMADigitalSource(dmaHandle, digitalSource->GetPortHandleForRouting(),
                          &status);
  FRC_CheckErrorStatus(status, "AddDigitalSource");
}

void DMA::AddDutyCycle(const DutyCycle* dutyCycle) {
  int32_t status = 0;
  HAL_AddDMADutyCycle(dmaHandle, dutyCycle->m_handle, &status);
  FRC_CheckErrorStatus(status, "AddDutyCycle");
}

void DMA::AddAnalogInput(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAnalogInput(dmaHandle, analogInput->m_port, &status);
  FRC_CheckErrorStatus(status, "AddAnalogInput");
}

void DMA::AddAveragedAnalogInput(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAveragedAnalogInput(dmaHandle, analogInput->m_port, &status);
  FRC_CheckErrorStatus(status, "AddAveragedAnalogInput");
}

void DMA::AddAnalogAccumulator(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAnalogAccumulator(dmaHandle, analogInput->m_port, &status);
  FRC_CheckErrorStatus(status, "AddAnalogAccumulator");
}

int DMA::SetExternalTrigger(DigitalSource* source, bool rising, bool falling) {
  int32_t status = 0;
  int32_t idx =
      HAL_SetDMAExternalTrigger(dmaHandle, source->GetPortHandleForRouting(),
                                static_cast<HAL_AnalogTriggerType>(
                                    source->GetAnalogTriggerTypeForRouting()),
                                rising, falling, &status);
  FRC_CheckErrorStatus(status, "SetExternalTrigger");
  return idx;
}

int DMA::SetPwmEdgeTrigger(PWMMotorController* source, bool rising,
                           bool falling) {
  int32_t status = 0;
  int32_t idx = HAL_SetDMAExternalTrigger(
      dmaHandle, source->GetPwm()->m_handle,
      HAL_AnalogTriggerType::HAL_Trigger_kInWindow, rising, falling, &status);
  FRC_CheckErrorStatus(status, "SetPWmEdgeTrigger");
  return idx;
}

int DMA::SetPwmEdgeTrigger(PWM* source, bool rising, bool falling) {
  int32_t status = 0;
  int32_t idx = HAL_SetDMAExternalTrigger(
      dmaHandle, source->m_handle, HAL_AnalogTriggerType::HAL_Trigger_kInWindow,
      rising, falling, &status);
  FRC_CheckErrorStatus(status, "SetPWmEdgeTrigger");
  return idx;
}

void DMA::ClearSensors() {
  int32_t status = 0;
  HAL_ClearDMASensors(dmaHandle, &status);
  FRC_CheckErrorStatus(status, "ClearSensors");
}

void DMA::ClearExternalTriggers() {
  int32_t status = 0;
  HAL_ClearDMAExternalTriggers(dmaHandle, &status);
  FRC_CheckErrorStatus(status, "ClearExternalTriggers");
}

void DMA::Start(int queueDepth) {
  int32_t status = 0;
  HAL_StartDMA(dmaHandle, queueDepth, &status);
  FRC_CheckErrorStatus(status, "Start");
}

void DMA::Stop() {
  int32_t status = 0;
  HAL_StopDMA(dmaHandle, &status);
  FRC_CheckErrorStatus(status, "Stop");
}
