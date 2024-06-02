// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Compressor.h"

#include <frc/PneumaticHub.h>

#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"

using namespace frc;

Compressor::Compressor(int module, PneumaticsModuleType moduleType)
    : m_module{PneumaticsBase::GetForType(module, moduleType)},
      m_moduleType{moduleType} {
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
  if (m_module) {
    m_module->UnreserveCompressor();
  }
}

bool Compressor::IsEnabled() const {
  return m_module->GetCompressor();
}

bool Compressor::GetPressureSwitchValue() const {
  return m_module->GetPressureSwitch();
}

units::ampere_t Compressor::GetCurrent() const {
  return m_module->GetCompressorCurrent();
}

units::volt_t Compressor::GetAnalogVoltage() const {
  return m_module->GetAnalogVoltage(0);
}

units::pounds_per_square_inch_t Compressor::GetPressure() const {
  return m_module->GetPressure(0);
}

void Compressor::Disable() {
  m_module->DisableCompressor();
}

void Compressor::EnableDigital() {
  m_module->EnableCompressorDigital();
}

void Compressor::EnableAnalog(units::pounds_per_square_inch_t minPressure,
                              units::pounds_per_square_inch_t maxPressure) {
  m_module->EnableCompressorAnalog(minPressure, maxPressure);
}

void Compressor::EnableHybrid(units::pounds_per_square_inch_t minPressure,
                              units::pounds_per_square_inch_t maxPressure) {
  m_module->EnableCompressorHybrid(minPressure, maxPressure);
}

CompressorConfigType Compressor::GetConfigType() const {
  return m_module->GetCompressorConfigType();
}

void Compressor::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Compressor");
  builder.AddBooleanProperty(
      "Enabled", [this] { return IsEnabled(); }, nullptr);
  builder.AddBooleanProperty(
      "Pressure switch", [this] { return GetPressureSwitchValue(); }, nullptr);
  builder.AddDoubleProperty(
      "Current (A)", [this] { return GetCurrent().value(); }, nullptr);
  // These are not supported by the CTRE PCM
  if (m_moduleType == PneumaticsModuleType::REVPH) {
    builder.AddDoubleProperty(
        "Analog Voltage", [this] { return GetAnalogVoltage().value(); },
        nullptr);
    builder.AddDoubleProperty(
        "Pressure (PSI)", [this] { return GetPressure().value(); }, nullptr);
  }
}
