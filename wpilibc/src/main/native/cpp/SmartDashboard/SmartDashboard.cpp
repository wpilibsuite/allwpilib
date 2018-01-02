/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SmartDashboard.h"

#include <llvm/StringMap.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <support/mutex.h>

#include "HLUsageReporting.h"
#include "SmartDashboard/Sendable.h"
#include "SmartDashboard/SendableBuilderImpl.h"
#include "WPIErrors.h"

using namespace frc;

namespace {
class SmartDashboardData {
 public:
  SmartDashboardData() = default;
  explicit SmartDashboardData(Sendable* sendable_) : sendable(sendable_) {}

  Sendable* sendable = nullptr;
  SendableBuilderImpl builder;
};

class Singleton {
 public:
  static Singleton& GetInstance();

  std::shared_ptr<nt::NetworkTable> table;
  llvm::StringMap<SmartDashboardData> tablesToData;
  wpi::mutex tablesToDataMutex;

 private:
  Singleton() {
    table = nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
    HLUsageReporting::ReportSmartDashboard();
  }
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
};

}  // namespace

Singleton& Singleton::GetInstance() {
  static Singleton instance;
  return instance;
}

void SmartDashboard::init() { Singleton::GetInstance(); }

/**
 * Determines whether the given key is in this table.
 *
 * @param key the key to search for
 * @return true if the table as a value assigned to the given key
 */
bool SmartDashboard::ContainsKey(llvm::StringRef key) {
  return Singleton::GetInstance().table->ContainsKey(key);
}

/**
 * @param types bitmask of types; 0 is treated as a "don't care".
 * @return keys currently in the table
 */
std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return Singleton::GetInstance().table->GetKeys(types);
}

/**
 * Makes a key's value persistent through program restarts.
 *
 * @param key the key to make persistent
 */
void SmartDashboard::SetPersistent(llvm::StringRef key) {
  Singleton::GetInstance().table->GetEntry(key).SetPersistent();
}

/**
 * Stop making a key's value persistent through program restarts.
 * The key cannot be null.
 *
 * @param key the key name
 */
void SmartDashboard::ClearPersistent(llvm::StringRef key) {
  Singleton::GetInstance().table->GetEntry(key).ClearPersistent();
}

/**
 * Returns whether the value is persistent through program restarts.
 * The key cannot be null.
 *
 * @param key the key name
 */
bool SmartDashboard::IsPersistent(llvm::StringRef key) {
  return Singleton::GetInstance().table->GetEntry(key).IsPersistent();
}

/**
 * Sets flags on the specified key in this table. The key can
 * not be null.
 *
 * @param key the key name
 * @param flags the flags to set (bitmask)
 */
void SmartDashboard::SetFlags(llvm::StringRef key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).SetFlags(flags);
}

/**
 * Clears flags on the specified key in this table. The key can
 * not be null.
 *
 * @param key the key name
 * @param flags the flags to clear (bitmask)
 */
void SmartDashboard::ClearFlags(llvm::StringRef key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).ClearFlags(flags);
}

/**
 * Returns the flags for the specified key.
 *
 * @param key the key name
 * @return the flags, or 0 if the key is not defined
 */
unsigned int SmartDashboard::GetFlags(llvm::StringRef key) {
  return Singleton::GetInstance().table->GetEntry(key).GetFlags();
}

/**
 * Deletes the specified key in this table.
 *
 * @param key the key name
 */
void SmartDashboard::Delete(llvm::StringRef key) {
  Singleton::GetInstance().table->Delete(key);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 */
void SmartDashboard::PutData(llvm::StringRef key, Sendable* data) {
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  auto& inst = Singleton::GetInstance();
  std::lock_guard<wpi::mutex> lock(inst.tablesToDataMutex);
  auto& sddata = inst.tablesToData[key];
  if (!sddata.sendable || sddata.sendable != data) {
    sddata = SmartDashboardData(data);
    auto dataTable = inst.table->GetSubTable(key);
    sddata.builder.SetTable(dataTable);
    data->InitSendable(sddata.builder);
    sddata.builder.UpdateTable();
    sddata.builder.StartListeners();
    dataTable->GetEntry(".name").SetString(key);
  }
}

/**
 * Maps the specified key (where the key is the name of the Sendable)
 * to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param value the value
 */
void SmartDashboard::PutData(Sendable* value) {
  if (value == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  PutData(value->GetName(), value);
}

/**
 * Returns the value at the specified key.
 *
 * @param keyName the key
 * @return the value
 */
Sendable* SmartDashboard::GetData(llvm::StringRef key) {
  auto& inst = Singleton::GetInstance();
  std::lock_guard<wpi::mutex> lock(inst.tablesToDataMutex);
  auto data = inst.tablesToData.find(key);
  if (data == inst.tablesToData.end()) {
    wpi_setGlobalWPIErrorWithContext(SmartDashboardMissingKey, key);
    return nullptr;
  }
  return data->getValue().sendable;
}

/**
 * Maps the specified key to the specified complex value (such as an array) in
 * this table.
 *
 * The value can be retrieved by calling the RetrieveValue method with a key
 * that is equal to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutValue(llvm::StringRef keyName,
                              std::shared_ptr<nt::Value> value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetValue(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key the key
 * @param defaultValue The default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultValue(llvm::StringRef key,
                                     std::shared_ptr<nt::Value> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultValue(
      defaultValue);
}

/**
 * Retrieves the complex value (such as an array) in this table into the complex
 * data object.
 *
 * @param keyName the key
 * @param value   the object to retrieve the value into
 */
std::shared_ptr<nt::Value> SmartDashboard::GetValue(llvm::StringRef keyName) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetValue();
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutBoolean(llvm::StringRef keyName, bool value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetBoolean(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultBoolean(llvm::StringRef key, bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBoolean(
      defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
bool SmartDashboard::GetBoolean(llvm::StringRef keyName, bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetBoolean(
      defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutNumber(llvm::StringRef keyName, double value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetDouble(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key          The key.
 * @param defaultValue The default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultNumber(llvm::StringRef key,
                                      double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDouble(
      defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
double SmartDashboard::GetNumber(llvm::StringRef keyName, double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetDouble(
      defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutString(llvm::StringRef keyName, llvm::StringRef value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetString(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultString(llvm::StringRef key,
                                      llvm::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultString(
      defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
std::string SmartDashboard::GetString(llvm::StringRef keyName,
                                      llvm::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetString(
      defaultValue);
}

/**
 * Put a boolean array in the table.
 *
 * @param key the key to be assigned to
 * @param value the value that will be assigned
 * @return False if the table key already exists with a different type
 *
 * @note The array must be of int's rather than of bool's because
 *       std::vector<bool> is special-cased in C++. 0 is false, any
 *       non-zero value is true.
 */
bool SmartDashboard::PutBooleanArray(llvm::StringRef key,
                                     llvm::ArrayRef<int> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetBooleanArray(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultBooleanArray(llvm::StringRef key,
                                            llvm::ArrayRef<int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBooleanArray(
      defaultValue);
}

/**
 * Returns the boolean array the key maps to.
 *
 * If the key does not exist or is of different type, it will return the default
 * value.
 *
 * @param key          The key to look up.
 * @param defaultValue The value to be returned if no value is found.
 * @return the value associated with the given key or the given default value
 *         if there is no value associated with the key
 *
 * @note This makes a copy of the array. If the overhead of this is a concern,
 *       use GetValue() instead.
 *
 * @note The returned array is std::vector<int> instead of std::vector<bool>
 *       because std::vector<bool> is special-cased in C++. 0 is false, any
 *       non-zero value is true.
 */
std::vector<int> SmartDashboard::GetBooleanArray(
    llvm::StringRef key, llvm::ArrayRef<int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetBooleanArray(
      defaultValue);
}

/**
 * Put a number array in the table.
 *
 * @param key   The key to be assigned to.
 * @param value The value that will be assigned.
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutNumberArray(llvm::StringRef key,
                                    llvm::ArrayRef<double> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetDoubleArray(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key          The key.
 * @param defaultValue The default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultNumberArray(
    llvm::StringRef key, llvm::ArrayRef<double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDoubleArray(
      defaultValue);
}

/**
 * Returns the number array the key maps to.
 *
 * If the key does not exist or is of different type, it will return the default
 * value.
 *
 * @param key The key to look up.
 * @param defaultValue The value to be returned if no value is found.
 * @return the value associated with the given key or the given default value
 * if there is no value associated with the key
 *
 * @note This makes a copy of the array. If the overhead of this is a concern,
 *       use GetValue() instead.
 */
std::vector<double> SmartDashboard::GetNumberArray(
    llvm::StringRef key, llvm::ArrayRef<double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetDoubleArray(
      defaultValue);
}

/**
 * Put a string array in the table.
 *
 * @param key   The key to be assigned to.
 * @param value The value that will be assigned.
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutStringArray(llvm::StringRef key,
                                    llvm::ArrayRef<std::string> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetStringArray(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key          The key.
 * @param defaultValue The default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultStringArray(
    llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultStringArray(
      defaultValue);
}

/**
 * Returns the string array the key maps to.
 *
 * If the key does not exist or is of different type, it will return the default
 * value.
 *
 * @param key          The key to look up.
 * @param defaultValue The value to be returned if no value is found.
 * @return the value associated with the given key or the given default value
 * if there is no value associated with the key
 *
 * @note This makes a copy of the array. If the overhead of this is a concern,
 *       use GetValue() instead.
 */
std::vector<std::string> SmartDashboard::GetStringArray(
    llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetStringArray(
      defaultValue);
}

/**
 * Put a raw value (byte array) in the table.
 *
 * @param key   The key to be assigned to.
 * @param value The value that will be assigned.
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutRaw(llvm::StringRef key, llvm::StringRef value) {
  return Singleton::GetInstance().table->GetEntry(key).SetRaw(value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 *
 * @param key          The key.
 * @param defaultValue The default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultRaw(llvm::StringRef key,
                                   llvm::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultRaw(
      defaultValue);
}

/**
 * Returns the raw value (byte array) the key maps to.
 *
 * If the key does not exist or is of different type, it will return the default
 * value.
 *
 * @param key          The key to look up.
 * @param defaultValue The value to be returned if no value is found.
 * @return the value associated with the given key or the given default value
 * if there is no value associated with the key
 *
 * @note This makes a copy of the raw contents. If the overhead of this is a
 *       concern, use GetValue() instead.
 */
std::string SmartDashboard::GetRaw(llvm::StringRef key,
                                   llvm::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetRaw(defaultValue);
}

/**
 * Puts all sendable data to the dashboard.
 */
void SmartDashboard::UpdateValues() {
  auto& inst = Singleton::GetInstance();
  std::lock_guard<wpi::mutex> lock(inst.tablesToDataMutex);
  for (auto& i : inst.tablesToData) {
    i.getValue().builder.UpdateTable();
  }
}
