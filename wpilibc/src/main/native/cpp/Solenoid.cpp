// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Solenoid.h"

#include <wpi/NullDeleter.h>
#include <wpi/telemetry/TelemetryTable.h>

#include "frc/Errors.h"

using namespace frc;

Solenoid::Solenoid(int busId, int module, PneumaticsModuleType moduleType,
                   int channel)
    : m_module{PneumaticsBase::GetForType(busId, module, moduleType)},
      m_channel{channel} {
  if (!m_module->CheckSolenoidChannel(m_channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", m_channel);
  }
  m_mask = 1 << channel;

  if (m_module->CheckAndReserveSolenoids(m_mask) != 0) {
    throw FRC_MakeError(err::ResourceAlreadyAllocated, "Channel {}", m_channel);
  }

  m_module->ReportUsage(fmt::format("Solenoid[{}]", m_channel), "Solenoid");
}

Solenoid::Solenoid(int busId, PneumaticsModuleType moduleType, int channel)
    : Solenoid{busId, PneumaticsBase::GetDefaultForType(moduleType), moduleType,
               channel} {}

Solenoid::~Solenoid() {
  if (m_module) {
    m_module->UnreserveSolenoids(m_mask);
  }
}

void Solenoid::Set(bool on) {
  int value = on ? (0xFFFF & m_mask) : 0;
  m_module->SetSolenoids(m_mask, value);
}

bool Solenoid::Get() const {
  int currentAll = m_module->GetSolenoids();
  return (currentAll & m_mask) != 0;
}

void Solenoid::Toggle() {
  Set(!Get());
}

int Solenoid::GetChannel() const {
  return m_channel;
}

bool Solenoid::IsDisabled() const {
  return (m_module->GetSolenoidDisabledList() & m_mask) != 0;
}

void Solenoid::SetPulseDuration(units::second_t duration) {
  m_module->SetOneShotDuration(m_channel, duration);
}

void Solenoid::StartPulse() {
  m_module->FireOneShot(m_channel);
}

void Solenoid::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", Get());
}

std::string_view Solenoid::GetTelemetryType() const {
  return "Solenoid";
}
