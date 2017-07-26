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

bool frc::Trigger::Grab() {
  if (Get()) {
    return true;
  } else if (m_table != nullptr) {
    return m_table->GetBoolean("pressed", false);
  } else {
    return false;
  }
}

void frc::Trigger::WhenActive(Command* command) {
  auto pbs = new PressedButtonScheduler(Grab(), this, command);
  pbs->Start();
}

void frc::Trigger::WhileActive(Command* command) {
  auto hbs = new HeldButtonScheduler(Grab(), this, command);
  hbs->Start();
}

void frc::Trigger::WhenInactive(Command* command) {
  auto rbs = new ReleasedButtonScheduler(Grab(), this, command);
  rbs->Start();
}

void frc::Trigger::CancelWhenActive(Command* command) {
  auto cbs = new CancelButtonScheduler(Grab(), this, command);
  cbs->Start();
}

void frc::Trigger::ToggleWhenActive(Command* command) {
  auto tbs = new ToggleButtonScheduler(Grab(), this, command);
  tbs->Start();
}

std::string frc::Trigger::GetSmartDashboardType() const { return "Button"; }

void frc::Trigger::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  if (m_table != nullptr) {
    m_table->PutBoolean("pressed", Get());
  }
}

std::shared_ptr<ITable> frc::Trigger::GetTable() const { return m_table; }
