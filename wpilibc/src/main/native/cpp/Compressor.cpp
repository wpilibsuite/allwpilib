// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Compressor.h"

#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"

using namespace frc;

Compressor::Compressor(int module, PneumaticsModuleType moduleType)
    : m_module{PneumaticsBase::GetForType(module, moduleType)} {
  if (!m_module->ReserveCompressor()) {
    throw FRC_MakeError(err::ResourceAlreadyAllocated, "{}", module);
  }

  SetClosedLoopControl(true);

  HAL_Report(HALUsageReporting::kResourceType_Compressor, module + 1);
  wpi::SendableRegistry::AddLW(this, "Compressor", module);
}

Compressor::Compressor(PneumaticsModuleType moduleType)
    : Compressor{PneumaticsBase::GetDefaultForType(moduleType), moduleType} {}

Compressor::~Compressor() {
  m_module->UnreserveCompressor();
}

void Compressor::Start() {
  SetClosedLoopControl(true);
}

void Compressor::Stop() {
  SetClosedLoopControl(false);
}

bool Compressor::Enabled() const {
  return m_module->GetCompressor();
}

bool Compressor::GetPressureSwitchValue() const {
  return m_module->GetPressureSwitch();
}

double Compressor::GetCompressorCurrent() const {
  return m_module->GetCompressorCurrent();
}

void Compressor::SetClosedLoopControl(bool on) {
  m_module->SetClosedLoopControl(on);
}

bool Compressor::GetClosedLoopControl() const {
  return m_module->GetClosedLoopControl();
}

void Compressor::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Compressor");
  builder.AddBooleanProperty(
      "Closed Loop Control", [=]() { return GetClosedLoopControl(); },
      [=](bool value) { SetClosedLoopControl(value); });
  builder.AddBooleanProperty(
      "Enabled", [=] { return Enabled(); }, nullptr);
  builder.AddBooleanProperty(
      "Pressure switch", [=]() { return GetPressureSwitchValue(); }, nullptr);
}
