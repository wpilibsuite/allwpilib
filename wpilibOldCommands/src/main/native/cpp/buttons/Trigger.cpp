// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/sendable/SendableBuilder.h>

#include "frc/buttons/Button.h"
#include "frc/buttons/CancelButtonScheduler.h"
#include "frc/buttons/HeldButtonScheduler.h"
#include "frc/buttons/PressedButtonScheduler.h"
#include "frc/buttons/ReleasedButtonScheduler.h"
#include "frc/buttons/ToggleButtonScheduler.h"

using namespace frc;

Trigger::Trigger(const Trigger& rhs) : SendableHelper(rhs) {}

Trigger& Trigger::operator=(const Trigger& rhs) {
  SendableHelper::operator=(rhs);
  m_sendablePressed = false;
  return *this;
}

Trigger::Trigger(Trigger&& rhs)
    : SendableHelper(std::move(rhs)),
      m_sendablePressed(rhs.m_sendablePressed.load()) {
  rhs.m_sendablePressed = false;
}

Trigger& Trigger::operator=(Trigger&& rhs) {
  SendableHelper::operator=(std::move(rhs));
  m_sendablePressed = rhs.m_sendablePressed.load();
  rhs.m_sendablePressed = false;
  return *this;
}

bool Trigger::Grab() {
  return Get() || m_sendablePressed;
}

void Trigger::WhenActive(Command* command) {
  auto pbs = new PressedButtonScheduler(Grab(), this, command);
  pbs->Start();
}

void Trigger::WhileActive(Command* command) {
  auto hbs = new HeldButtonScheduler(Grab(), this, command);
  hbs->Start();
}

void Trigger::WhenInactive(Command* command) {
  auto rbs = new ReleasedButtonScheduler(Grab(), this, command);
  rbs->Start();
}

void Trigger::CancelWhenActive(Command* command) {
  auto cbs = new CancelButtonScheduler(Grab(), this, command);
  cbs->Start();
}

void Trigger::ToggleWhenActive(Command* command) {
  auto tbs = new ToggleButtonScheduler(Grab(), this, command);
  tbs->Start();
}

void Trigger::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Button");
  builder.SetSafeState([=] { m_sendablePressed = false; });
  builder.AddBooleanProperty(
      "pressed", [=] { return Grab(); },
      [=](bool value) { m_sendablePressed = value; });
}
