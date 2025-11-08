// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/pneumatic/Compressor.hpp"

#include "wpi/hal/Ports.h"
#include "wpi/hardware/pneumatic/PneumaticHub.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

Compressor::Compressor(int busId, int module, PneumaticsModuleType moduleType)
    : m_module{PneumaticsBase::GetForType(busId, module, moduleType)},
      m_moduleType{moduleType} {
  if (!m_module->ReserveCompressor()) {
    throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "{}", module);
  }

  m_module->EnableCompressorDigital();

  m_module->ReportUsage("Compressor", "");
  wpi::util::SendableRegistry::Add(this, "Compressor", module);
}

Compressor::Compressor(int busId, PneumaticsModuleType moduleType)
    : Compressor{busId, PneumaticsBase::GetDefaultForType(moduleType),
                 moduleType} {}

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

wpi::units::ampere_t Compressor::GetCurrent() const {
  return m_module->GetCompressorCurrent();
}

wpi::units::volt_t Compressor::GetAnalogVoltage() const {
  return m_module->GetAnalogVoltage(0);
}

wpi::units::pounds_per_square_inch_t Compressor::GetPressure() const {
  return m_module->GetPressure(0);
}

void Compressor::Disable() {
  m_module->DisableCompressor();
}

void Compressor::EnableDigital() {
  m_module->EnableCompressorDigital();
}

void Compressor::EnableAnalog(
    wpi::units::pounds_per_square_inch_t minPressure,
    wpi::units::pounds_per_square_inch_t maxPressure) {
  m_module->EnableCompressorAnalog(minPressure, maxPressure);
}

void Compressor::EnableHybrid(
    wpi::units::pounds_per_square_inch_t minPressure,
    wpi::units::pounds_per_square_inch_t maxPressure) {
  m_module->EnableCompressorHybrid(minPressure, maxPressure);
}

CompressorConfigType Compressor::GetConfigType() const {
  return m_module->GetCompressorConfigType();
}

void Compressor::InitSendable(wpi::util::SendableBuilder& builder) {
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
