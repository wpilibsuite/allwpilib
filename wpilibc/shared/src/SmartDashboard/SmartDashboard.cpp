/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SmartDashboard.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "SmartDashboard/NamedSendable.h"
#include "WPIErrors.h"
#include "networktables/NetworkTable.h"
#include "HLUsageReporting.h"

std::shared_ptr<ITable> SmartDashboard::m_table;
std::map<std::shared_ptr<ITable> , Sendable *> SmartDashboard::m_tablesToData;

void SmartDashboard::init() {
  m_table = NetworkTable::GetTable("SmartDashboard");

  HLUsageReporting::ReportSmartDashboard();
}

/**
 * Maps the specified key to the specified value in this table.
 * The key can not be nullptr.
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 * @param keyName the key
 * @param value the value
 */
void SmartDashboard::PutData(llvm::StringRef key, Sendable *data) {
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  std::shared_ptr<ITable> dataTable(m_table->GetSubTable(key));
  dataTable->PutString("~TYPE~", data->GetSmartDashboardType());
  data->InitTable(dataTable);
  m_tablesToData[dataTable] = data;
}

/**
 * Maps the specified key (where the key is the name of the {@link
 * SmartDashboardNamedData}
 * to the specified value in this table.
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 * @param value the value
 */
void SmartDashboard::PutData(NamedSendable *value) {
  if (value == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  PutData(value->GetName(), value);
}

/**
 * Returns the value at the specified key.
 * @param keyName the key
 * @return the value
 */
Sendable *SmartDashboard::GetData(llvm::StringRef key) {
  std::shared_ptr<ITable> subtable(m_table->GetSubTable(key));
  Sendable *data = m_tablesToData[subtable];
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(SmartDashboardMissingKey, key);
    return nullptr;
  }
  return data;
}

/**
 * Maps the specified key to the specified complex value (such as an array) in
 * this table.
 * The key can not be nullptr.
 * The value can be retrieved by calling the RetrieveValue method with a key
 * that is equal to the original key.
 * @param keyName the key
 * @param value the value
 */
void SmartDashboard::PutValue(llvm::StringRef keyName,
                              std::shared_ptr<nt::Value> value) {
  m_table->PutValue(keyName, value);
}

/**
 * Retrieves the complex value (such as an array) in this table into the complex
 * data object
 * The key can not be nullptr.
 * @param keyName the key
 * @param value the object to retrieve the value into
 */
std::shared_ptr<nt::Value> SmartDashboard::GetValue(llvm::StringRef keyName) {
  return m_table->GetValue(keyName);
}

/**
 * Maps the specified key to the specified value in this table.
 * The key can not be nullptr.
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 * @param keyName the key
 * @param value the value
 */
void SmartDashboard::PutBoolean(llvm::StringRef keyName, bool value) {
  m_table->PutBoolean(keyName, value);
}

/**
 * Returns the value at the specified key. If the key is not found, returns the
 * default value.
 * @param keyName the key
 * @return the value
 */
bool SmartDashboard::GetBoolean(llvm::StringRef keyName, bool defaultValue) {
  return m_table->GetBoolean(keyName, defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 * The key can not be nullptr.
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 * @param keyName the key
 * @param value the value
 */
void SmartDashboard::PutNumber(llvm::StringRef keyName, double value) {
  m_table->PutNumber(keyName, value);
}

/**
 * Returns the value at the specified key. If the key is not found, returns the
 * default value.
 * @param keyName the key
 * @return the value
 */
double SmartDashboard::GetNumber(llvm::StringRef keyName, double defaultValue) {
  return m_table->GetNumber(keyName, defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 * Neither the key nor the value can be nullptr.
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 * @param keyName the key
 * @param value the value
 */
void SmartDashboard::PutString(llvm::StringRef keyName, llvm::StringRef value) {
  m_table->PutString(keyName, value);
}

/**
 * Returns the value at the specified key. If the key is not found, returns the
 * default value.
 * @param keyName the key
 * @return the value
 */
std::string SmartDashboard::GetString(llvm::StringRef keyName,
                                      llvm::StringRef defaultValue) {
  return m_table->GetString(keyName, defaultValue);
}
