// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DMA.h"

#include <frc/AnalogInput.h>
#include <frc/Counter.h>
#include <frc/DigitalSource.h>
#include <frc/DutyCycle.h>
#include <frc/Encoder.h>

#include <hal/DMA.h>
#include <hal/HALBase.h>

using namespace frc;

DMA::DMA() {
  int32_t status = 0;
  dmaHandle = HAL_InitializeDMA(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

DMA::~DMA() { HAL_FreeDMA(dmaHandle); }

void DMA::SetPause(bool pause) {
  int32_t status = 0;
  HAL_SetDMAPause(dmaHandle, pause, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::SetRate(int cycles) {
  int32_t status = 0;
  HAL_SetDMARate(dmaHandle, cycles, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddEncoder(const Encoder* encoder) {
  int32_t status = 0;
  HAL_AddDMAEncoder(dmaHandle, encoder->m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddEncoderPeriod(const Encoder* encoder) {
  int32_t status = 0;
  HAL_AddDMAEncoderPeriod(dmaHandle, encoder->m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddCounter(const Counter* counter) {
  int32_t status = 0;
  HAL_AddDMACounter(dmaHandle, counter->m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddCounterPeriod(const Counter* counter) {
  int32_t status = 0;
  HAL_AddDMACounterPeriod(dmaHandle, counter->m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddDigitalSource(const DigitalSource* digitalSource) {
  int32_t status = 0;
  HAL_AddDMADigitalSource(dmaHandle, digitalSource->GetPortHandleForRouting(),
                          &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddDutyCycle(const DutyCycle* dutyCycle) {
  int32_t status = 0;
  HAL_AddDMADutyCycle(dmaHandle, dutyCycle->m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddAnalogInput(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAnalogInput(dmaHandle, analogInput->m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddAveragedAnalogInput(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAveragedAnalogInput(dmaHandle, analogInput->m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::AddAnalogAccumulator(const AnalogInput* analogInput) {
  int32_t status = 0;
  HAL_AddDMAAnalogAccumulator(dmaHandle, analogInput->m_port, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::SetExternalTrigger(DigitalSource* source, bool rising, bool falling) {
  int32_t status = 0;
  HAL_SetDMAExternalTrigger(dmaHandle, source->GetPortHandleForRouting(),
                            static_cast<HAL_AnalogTriggerType>(
                                source->GetAnalogTriggerTypeForRouting()),
                            rising, falling, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::StartDMA(int queueDepth) {
  int32_t status = 0;
  HAL_StartDMA(dmaHandle, queueDepth, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void DMA::StopDMA() {
  int32_t status = 0;
  HAL_StopDMA(dmaHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}
