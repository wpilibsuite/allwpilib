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

  m_module->EnableCompressorDigital();

  HAL_Report(HALUsageReporting::kResourceType_Compressor, module + 1);
  wpi::SendableRegistry::AddLW(this, "Compressor", module);
}

Compressor::Compressor(PneumaticsModuleType moduleType)
    : Compressor{PneumaticsBase::GetDefaultForType(moduleType), moduleType} {}

Compressor::~Compressor() {
  m_module->UnreserveCompressor();
}

void Compressor::Start() {
  EnableDigital();
}

void Compressor::Stop() {
  Disable();
}

bool Compressor::Enabled() const {
  return m_module->GetCompressor();
}

bool Compressor::GetPressureSwitchValue() const {
  return m_module->GetPressureSwitch();
}

double Compressor::GetCurrent() const {
  return m_module->GetCompressorCurrent();
}

void Compressor::Disable() {
  m_module->DisableCompressor();
}

void Compressor::EnableDigital() {
  m_module->EnableCompressorDigital();
}

void Compressor::EnableAnalog(double minAnalogVoltage,
                              double maxAnalogVoltage) {
  m_module->EnableCompressorAnalog(minAnalogVoltage, maxAnalogVoltage);
}

void Compressor::EnableHybrid(double minAnalogVoltage,
                              double maxAnalogVoltage) {
  m_module->EnableCompressorHybrid(minAnalogVoltage, maxAnalogVoltage);
}

CompressorConfigType Compressor::GetConfigType() const {
  return m_module->GetCompressorConfigType();
}

void Compressor::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Compressor");
  builder.AddBooleanProperty(
      "Enabled", [=] { return Enabled(); }, nullptr);
  builder.AddBooleanProperty(
      "Pressure switch", [=]() { return GetPressureSwitchValue(); }, nullptr);
}
