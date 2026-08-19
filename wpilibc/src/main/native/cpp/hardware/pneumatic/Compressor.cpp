// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/pneumatic/Compressor.hpp"

#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

Compressor::Compressor(CANBus busId, int module,
                       PneumaticsModuleType moduleType)
    : m_module{PneumaticsBase::GetForType(busId, module, moduleType)},
      m_moduleType{moduleType} {
  if (!m_module->ReserveCompressor()) {
    throw WPILIB_MakeError(err::ResourceAlreadyAllocated, "{}", module);
  }

  m_module->EnableCompressorDigital();

  m_module->ReportUsage("Compressor", "");
}

Compressor::Compressor(CANBus busId, PneumaticsModuleType moduleType)
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

wpi::units::amperes<> Compressor::GetCurrent() const {
  return m_module->GetCompressorCurrent();
}

wpi::units::volts<> Compressor::GetAnalogVoltage() const {
  return m_module->GetAnalogVoltage(0);
}

wpi::units::pounds_per_square_inch<> Compressor::GetPressure() const {
  return m_module->GetPressure(0);
}

void Compressor::Disable() {
  m_module->DisableCompressor();
}

void Compressor::EnableDigital() {
  m_module->EnableCompressorDigital();
}

void Compressor::EnableAnalog(
    wpi::units::pounds_per_square_inch<> minPressure,
    wpi::units::pounds_per_square_inch<> maxPressure) {
  m_module->EnableCompressorAnalog(minPressure, maxPressure);
}

void Compressor::EnableHybrid(
    wpi::units::pounds_per_square_inch<> minPressure,
    wpi::units::pounds_per_square_inch<> maxPressure) {
  m_module->EnableCompressorHybrid(minPressure, maxPressure);
}

CompressorConfigType Compressor::GetConfigType() const {
  return m_module->GetCompressorConfigType();
}

void Compressor::LogTo(wpi::telemetry::TelemetryTable& table) const {
  table.Log("Enabled", IsEnabled());
  table.Log("Pressure switch", GetPressureSwitchValue());
  table.Log("Current (A)", GetCurrent());
  // These are not supported by the CTRE PCM
  if (m_moduleType == PneumaticsModuleType::REV_PH) {
    table.Log("Analog Voltage", GetAnalogVoltage());
    table.Log("Pressure (PSI)", GetPressure());
  }
}

std::string_view Compressor::GetTelemetryType() const {
  return "Compressor";
}
