/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/NetworkButton.h"
#include "networktables/NetworkTable.h"

NetworkButton::NetworkButton(const std::string &tableName, const std::string &field)
    :  // TODO how is this supposed to work???
      m_netTable(NetworkTable::GetTable(tableName)),
      m_field(field) {}

NetworkButton::NetworkButton(std::shared_ptr<ITable> table, const std::string &field)
    : m_netTable(table), m_field(field) {}

bool NetworkButton::Get() {
  /*if (m_netTable->isConnected())
          return m_netTable->GetBoolean(m_field.c_str());
  else
          return false;*/
  return m_netTable->GetBoolean(m_field, false);
}
