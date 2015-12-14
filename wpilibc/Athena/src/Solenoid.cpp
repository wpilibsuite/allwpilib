/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

#include <sstream>

/**
 * Constructor using the default PCM ID (0).
 *
 * @param channel The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(uint32_t channel)
    : Solenoid(GetDefaultSolenoidModule(), channel) {}

/**
 * Constructor.
 *
 * @param moduleNumber The CAN ID of the PCM the solenoid is attached to
 * @param channel The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(uint8_t moduleNumber, uint32_t channel)
    : SolenoidBase(moduleNumber), m_channel(channel) {
  std::stringstream buf;
  if (!CheckSolenoidModule(m_moduleNumber)) {
    buf << "Solenoid Module " << m_moduleNumber;
    wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf.str());
    return;
  }
  if (!CheckSolenoidChannel(m_channel)) {
    buf << "Solenoid Module " << m_channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    return;
  }
  Resource::CreateResourceObject(m_allocated, m_maxModules * m_maxPorts);
  buf << "Solenoid " << m_channel << " (Module: " << m_moduleNumber << ")";
  if (m_allocated->Allocate(m_moduleNumber * kSolenoidChannels + m_channel,
                            buf.str()) ==
      std::numeric_limits<uint32_t>::max()) {
    CloneError(*m_allocated);
    return;
  }

  LiveWindow::GetInstance()->AddActuator("Solenoid", m_moduleNumber, m_channel,
                                         this);
  HALReport(HALUsageReporting::kResourceType_Solenoid, m_channel,
            m_moduleNumber);
}

/**
 * Destructor.
 */
Solenoid::~Solenoid() {
  if (CheckSolenoidModule(m_moduleNumber)) {
    m_allocated->Free(m_moduleNumber * kSolenoidChannels + m_channel);
  }
  if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Set the value of a solenoid.
 *
 * @param on Turn the solenoid output off or on.
 */
void Solenoid::Set(bool on) {
  if (StatusIsFatal()) return;
  uint8_t value = on ? 0xFF : 0x00;
  uint8_t mask = 1 << m_channel;

  SolenoidBase::Set(value, mask, m_moduleNumber);
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
bool Solenoid::Get() const {
  if (StatusIsFatal()) return false;
  uint8_t value = GetAll(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}
/**
 * Check if solenoid is blacklisted.
 * 		If a solenoid is shorted, it is added to the blacklist and
 * 		disabled until power cycle, or until faults are cleared.
 * 		@see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool Solenoid::IsBlackListed() const {
  int value = GetPCMSolenoidBlackList(m_moduleNumber) & (1 << m_channel);
  return (value != 0);
}

void Solenoid::ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsBoolean()) return;
  Set(value->GetBoolean());
}

void Solenoid::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutBoolean("Value", Get());
  }
}

void Solenoid::StartLiveWindowMode() {
  Set(false);
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void Solenoid::StopLiveWindowMode() {
  Set(false);
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string Solenoid::GetSmartDashboardType() const { return "Solenoid"; }

void Solenoid::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Solenoid::GetTable() const { return m_table; }
