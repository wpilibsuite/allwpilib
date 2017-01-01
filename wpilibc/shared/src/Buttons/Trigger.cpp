/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/Button.h"
#include "Buttons/CancelButtonScheduler.h"
#include "Buttons/HeldButtonScheduler.h"
#include "Buttons/PressedButtonScheduler.h"
#include "Buttons/ReleasedButtonScheduler.h"
#include "Buttons/ToggleButtonScheduler.h"

using namespace frc;

bool Trigger::Grab() {
  if (Get()) {
    return true;
  } else if (m_table != nullptr) {
    return m_table->GetBoolean("pressed", false);
  } else {
    return false;
  }
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

std::string Trigger::GetSmartDashboardType() const { return "Button"; }

void Trigger::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  if (m_table != nullptr) {
    m_table->PutBoolean("pressed", Get());
  }
}

std::shared_ptr<ITable> Trigger::GetTable() const { return m_table; }
