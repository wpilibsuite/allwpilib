/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"

#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"
#include "simulation/simTime.h"

/**
 * Constructor.
 *
 * @param channel The channel on the solenoid module to control (1..8).
 */
frc::Solenoid::Solenoid(int channel) : Solenoid(1, channel) {}

/**
 * Constructor.
 *
 * @param moduleNumber The solenoid module (1 or 2).
 * @param channel      The channel on the solenoid module to control (1..8).
 */
frc::Solenoid::Solenoid(int moduleNumber, int channel) {
  llvm::SmallString<32> buf;
  llvm::raw_svector_ostream oss(buf);
  oss << "pneumatic/" << moduleNumber << "/" << channel;
  m_impl = new SimContinuousOutput(oss.str());

  LiveWindow::GetInstance()->AddActuator("Solenoid", moduleNumber, channel,
                                         this);
}

frc::Solenoid::~Solenoid() {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Set the value of a solenoid.
 *
 * @param on Turn the solenoid output off or on.
 */
void frc::Solenoid::Set(bool on) {
  m_on = on;
  m_impl->Set(on ? 1 : -1);
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
bool frc::Solenoid::Get() const { return m_on; }

void frc::Solenoid::ValueChanged(ITable* source, llvm::StringRef key,
                                 std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsBoolean()) return;
  Set(value->GetBoolean());
}

void frc::Solenoid::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutBoolean("Value", Get());
  }
}

void frc::Solenoid::StartLiveWindowMode() {
  Set(false);
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void frc::Solenoid::StopLiveWindowMode() {
  Set(false);
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string frc::Solenoid::GetSmartDashboardType() const { return "Solenoid"; }

void frc::Solenoid::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> frc::Solenoid::GetTable() const { return m_table; }
