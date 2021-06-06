// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_INC_
#define NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_INC_

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "networktables/NetworkTableEntry.h"

namespace nt {

inline NetworkTableEntry::NetworkTableEntry() {}

inline NetworkTableEntry::NetworkTableEntry(NT_Entry handle)
    : m_handle{handle} {}

inline NT_Entry NetworkTableEntry::GetHandle() const {
  return m_handle;
}

inline bool NetworkTableEntry::Exists() const {
  return GetEntryType(m_handle) != NT_UNASSIGNED;
}

inline std::string NetworkTableEntry::GetName() const {
  return GetEntryName(m_handle);
}

inline NetworkTableType NetworkTableEntry::GetType() const {
  return static_cast<NetworkTableType>(GetEntryType(m_handle));
}

inline unsigned int NetworkTableEntry::GetFlags() const {
  return GetEntryFlags(m_handle);
}

inline uint64_t NetworkTableEntry::GetLastChange() const {
  return GetEntryLastChange(m_handle);
}

inline EntryInfo NetworkTableEntry::GetInfo() const {
  return GetEntryInfo(m_handle);
}

inline std::shared_ptr<Value> NetworkTableEntry::GetValue() const {
  return GetEntryValue(m_handle);
}

inline bool NetworkTableEntry::GetBoolean(bool defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_BOOLEAN) {
    return defaultValue;
  }
  return value->GetBoolean();
}

inline double NetworkTableEntry::GetDouble(double defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_DOUBLE) {
    return defaultValue;
  }
  return value->GetDouble();
}

inline std::string NetworkTableEntry::GetString(
    std::string_view defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_STRING) {
    return std::string{defaultValue};
  }
  return std::string{value->GetString()};
}

inline std::string NetworkTableEntry::GetRaw(
    std::string_view defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_RAW) {
    return std::string{defaultValue};
  }
  return std::string{value->GetRaw()};
}

inline std::vector<int> NetworkTableEntry::GetBooleanArray(
    wpi::ArrayRef<int> defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_BOOLEAN_ARRAY) {
    return defaultValue;
  }
  return value->GetBooleanArray();
}

inline std::vector<int> NetworkTableEntry::GetBooleanArray(
    std::initializer_list<int> defaultValue) const {
  return GetBooleanArray(
      wpi::makeArrayRef(defaultValue.begin(), defaultValue.end()));
}

inline std::vector<double> NetworkTableEntry::GetDoubleArray(
    wpi::ArrayRef<double> defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_DOUBLE_ARRAY) {
    return defaultValue;
  }
  return value->GetDoubleArray();
}

inline std::vector<double> NetworkTableEntry::GetDoubleArray(
    std::initializer_list<double> defaultValue) const {
  return GetDoubleArray(
      wpi::makeArrayRef(defaultValue.begin(), defaultValue.end()));
}

inline std::vector<std::string> NetworkTableEntry::GetStringArray(
    wpi::ArrayRef<std::string> defaultValue) const {
  auto value = GetEntryValue(m_handle);
  if (!value || value->type() != NT_STRING_ARRAY) {
    return defaultValue;
  }
  return value->GetStringArray();
}

inline std::vector<std::string> NetworkTableEntry::GetStringArray(
    std::initializer_list<std::string> defaultValue) const {
  return GetStringArray(
      wpi::makeArrayRef(defaultValue.begin(), defaultValue.end()));
}

inline bool NetworkTableEntry::SetDefaultValue(std::shared_ptr<Value> value) {
  return SetDefaultEntryValue(m_handle, value);
}

inline bool NetworkTableEntry::SetDefaultBoolean(bool defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeBoolean(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultDouble(double defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeDouble(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultString(std::string_view defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeString(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultRaw(std::string_view defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeRaw(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultBooleanArray(
    wpi::ArrayRef<int> defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeBooleanArray(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultBooleanArray(
    std::initializer_list<int> defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeBooleanArray(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultDoubleArray(
    wpi::ArrayRef<double> defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeDoubleArray(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultDoubleArray(
    std::initializer_list<double> value) {
  return SetDefaultEntryValue(m_handle, Value::MakeDoubleArray(value));
}

inline bool NetworkTableEntry::SetDefaultStringArray(
    wpi::ArrayRef<std::string> defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeStringArray(defaultValue));
}

inline bool NetworkTableEntry::SetDefaultStringArray(
    std::initializer_list<std::string> defaultValue) {
  return SetDefaultEntryValue(m_handle, Value::MakeStringArray(defaultValue));
}

inline bool NetworkTableEntry::SetValue(std::shared_ptr<Value> value) {
  return SetEntryValue(m_handle, value);
}

inline bool NetworkTableEntry::SetBoolean(bool value) {
  return SetEntryValue(m_handle, Value::MakeBoolean(value));
}

inline bool NetworkTableEntry::SetDouble(double value) {
  return SetEntryValue(m_handle, Value::MakeDouble(value));
}

inline bool NetworkTableEntry::SetString(std::string_view value) {
  return SetEntryValue(m_handle, Value::MakeString(value));
}

inline bool NetworkTableEntry::SetRaw(std::string_view value) {
  return SetEntryValue(m_handle, Value::MakeRaw(value));
}

inline bool NetworkTableEntry::SetBooleanArray(wpi::ArrayRef<bool> value) {
  return SetEntryValue(m_handle, Value::MakeBooleanArray(value));
}

inline bool NetworkTableEntry::SetBooleanArray(
    std::initializer_list<bool> value) {
  return SetEntryValue(m_handle, Value::MakeBooleanArray(value));
}

inline bool NetworkTableEntry::SetBooleanArray(wpi::ArrayRef<int> value) {
  return SetEntryValue(m_handle, Value::MakeBooleanArray(value));
}

inline bool NetworkTableEntry::SetBooleanArray(
    std::initializer_list<int> value) {
  return SetEntryValue(m_handle, Value::MakeBooleanArray(value));
}

inline bool NetworkTableEntry::SetDoubleArray(wpi::ArrayRef<double> value) {
  return SetEntryValue(m_handle, Value::MakeDoubleArray(value));
}

inline bool NetworkTableEntry::SetDoubleArray(
    std::initializer_list<double> value) {
  return SetEntryValue(m_handle, Value::MakeDoubleArray(value));
}

inline bool NetworkTableEntry::SetStringArray(
    wpi::ArrayRef<std::string> value) {
  return SetEntryValue(m_handle, Value::MakeStringArray(value));
}

inline bool NetworkTableEntry::SetStringArray(
    std::initializer_list<std::string> value) {
  return SetEntryValue(m_handle, Value::MakeStringArray(value));
}

inline void NetworkTableEntry::ForceSetValue(std::shared_ptr<Value> value) {
  SetEntryTypeValue(m_handle, value);
}

inline void NetworkTableEntry::ForceSetBoolean(bool value) {
  SetEntryTypeValue(m_handle, Value::MakeBoolean(value));
}

inline void NetworkTableEntry::ForceSetDouble(double value) {
  SetEntryTypeValue(m_handle, Value::MakeDouble(value));
}

inline void NetworkTableEntry::ForceSetString(std::string_view value) {
  SetEntryTypeValue(m_handle, Value::MakeString(value));
}

inline void NetworkTableEntry::ForceSetRaw(std::string_view value) {
  SetEntryTypeValue(m_handle, Value::MakeRaw(value));
}

inline void NetworkTableEntry::ForceSetBooleanArray(wpi::ArrayRef<bool> value) {
  SetEntryTypeValue(m_handle, Value::MakeBooleanArray(value));
}

inline void NetworkTableEntry::ForceSetBooleanArray(
    std::initializer_list<bool> value) {
  SetEntryTypeValue(m_handle, Value::MakeBooleanArray(value));
}

inline void NetworkTableEntry::ForceSetBooleanArray(wpi::ArrayRef<int> value) {
  SetEntryTypeValue(m_handle, Value::MakeBooleanArray(value));
}

inline void NetworkTableEntry::ForceSetBooleanArray(
    std::initializer_list<int> value) {
  SetEntryTypeValue(m_handle, Value::MakeBooleanArray(value));
}

inline void NetworkTableEntry::ForceSetDoubleArray(
    wpi::ArrayRef<double> value) {
  SetEntryTypeValue(m_handle, Value::MakeDoubleArray(value));
}

inline void NetworkTableEntry::ForceSetDoubleArray(
    std::initializer_list<double> value) {
  SetEntryTypeValue(m_handle, Value::MakeDoubleArray(value));
}

inline void NetworkTableEntry::ForceSetStringArray(
    wpi::ArrayRef<std::string> value) {
  SetEntryTypeValue(m_handle, Value::MakeStringArray(value));
}

inline void NetworkTableEntry::ForceSetStringArray(
    std::initializer_list<std::string> value) {
  SetEntryTypeValue(m_handle, Value::MakeStringArray(value));
}

inline void NetworkTableEntry::SetFlags(unsigned int flags) {
  SetEntryFlags(m_handle, GetFlags() | flags);
}

inline void NetworkTableEntry::ClearFlags(unsigned int flags) {
  SetEntryFlags(m_handle, GetFlags() & ~flags);
}

inline void NetworkTableEntry::SetPersistent() {
  SetFlags(kPersistent);
}

inline void NetworkTableEntry::ClearPersistent() {
  ClearFlags(kPersistent);
}

inline bool NetworkTableEntry::IsPersistent() const {
  return (GetFlags() & kPersistent) != 0;
}

inline void NetworkTableEntry::Delete() {
  DeleteEntry(m_handle);
}

inline void NetworkTableEntry::CreateRpc(
    std::function<void(const RpcAnswer& answer)> callback) {
  ::nt::CreateRpc(m_handle, std::string_view("\0", 1), callback);
}

inline RpcCall NetworkTableEntry::CallRpc(std::string_view params) {
  return RpcCall{m_handle, ::nt::CallRpc(m_handle, params)};
}

inline NT_EntryListener NetworkTableEntry::AddListener(
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) const {
  return AddEntryListener(m_handle, callback, flags);
}

inline void NetworkTableEntry::RemoveListener(NT_EntryListener entry_listener) {
  RemoveEntryListener(entry_listener);
}

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_INC_
