// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Compressor.h"

#include <hal/Compressor.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <hal/Solenoid.h>

#include "frc/Errors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Compressor::Compressor(int pcmID) : m_module(pcmID) {
  int32_t status = 0;
  m_compressorHandle = HAL_InitializeCompressor(m_module, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  SetClosedLoopControl(true);

  HAL_Report(HALUsageReporting::kResourceType_Compressor, pcmID + 1);
  SendableRegistry::GetInstance().AddLW(this, "Compressor", pcmID);
}

void Compressor::Start() {
  SetClosedLoopControl(true);
}

void Compressor::Stop() {
  SetClosedLoopControl(false);
}

bool Compressor::Enabled() const {
  int32_t status = 0;
  bool value = HAL_GetCompressor(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetPressureSwitchValue() const {
  int32_t status = 0;
  bool value = HAL_GetCompressorPressureSwitch(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

double Compressor::GetCompressorCurrent() const {
  int32_t status = 0;
  double value = HAL_GetCompressorCurrent(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

void Compressor::SetClosedLoopControl(bool on) {
  int32_t status = 0;
  HAL_SetCompressorClosedLoopControl(m_compressorHandle, on, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
}

bool Compressor::GetClosedLoopControl() const {
  int32_t status = 0;
  bool value = HAL_GetCompressorClosedLoopControl(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorCurrentTooHighFault() const {
  int32_t status = 0;
  bool value =
      HAL_GetCompressorCurrentTooHighFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorCurrentTooHighStickyFault() const {
  int32_t status = 0;
  bool value =
      HAL_GetCompressorCurrentTooHighStickyFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorShortedStickyFault() const {
  int32_t status = 0;
  bool value = HAL_GetCompressorShortedStickyFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorShortedFault() const {
  int32_t status = 0;
  bool value = HAL_GetCompressorShortedFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorNotConnectedStickyFault() const {
  int32_t status = 0;
  bool value =
      HAL_GetCompressorNotConnectedStickyFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

bool Compressor::GetCompressorNotConnectedFault() const {
  int32_t status = 0;
  bool value = HAL_GetCompressorNotConnectedFault(m_compressorHandle, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
  return value;
}

void Compressor::ClearAllPCMStickyFaults() {
  int32_t status = 0;
  HAL_ClearAllPCMStickyFaults(m_module, &status);
  FRC_CheckErrorStatus(status, "Module " + wpi::Twine{m_module});
}

int Compressor::GetModule() const {
  return m_module;
}

void Compressor::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Compressor");
  builder.AddBooleanProperty(
      "Closed Loop Control", [=]() { return GetClosedLoopControl(); },
      [=](bool value) { SetClosedLoopControl(value); });
  builder.AddBooleanProperty(
      "Enabled", [=] { return Enabled(); }, nullptr);
  builder.AddBooleanProperty(
      "Pressure switch", [=]() { return GetPressureSwitchValue(); }, nullptr);
}
