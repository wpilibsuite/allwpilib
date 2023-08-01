// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "networktables/NetworkTableEntry.h"
#include "ntcore_cpp.h"
#include "ntcore_cpp_types.h"

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

inline int64_t NetworkTableEntry::GetLastChange() const {
  return GetEntryLastChange(m_handle);
}

inline Value NetworkTableEntry::GetValue() const {
  return GetEntryValue(m_handle);
}

inline bool NetworkTableEntry::GetBoolean(bool defaultValue) const {
  return nt::GetBoolean(m_handle, defaultValue);
}

inline int64_t NetworkTableEntry::GetInteger(int64_t defaultValue) const {
  return nt::GetInteger(m_handle, defaultValue);
}

inline float NetworkTableEntry::GetFloat(float defaultValue) const {
  return nt::GetFloat(m_handle, defaultValue);
}

inline double NetworkTableEntry::GetDouble(double defaultValue) const {
  return nt::GetDouble(m_handle, defaultValue);
}

inline std::string NetworkTableEntry::GetString(
    std::string_view defaultValue) const {
  return nt::GetString(m_handle, defaultValue);
}

inline std::vector<uint8_t> NetworkTableEntry::GetRaw(
    std::span<const uint8_t> defaultValue) const {
  return nt::GetRaw(m_handle, defaultValue);
}

inline std::vector<int> NetworkTableEntry::GetBooleanArray(
    std::span<const int> defaultValue) const {
  return nt::GetBooleanArray(m_handle, defaultValue);
}

inline std::vector<int64_t> NetworkTableEntry::GetIntegerArray(
    std::span<const int64_t> defaultValue) const {
  return nt::GetIntegerArray(m_handle, defaultValue);
}

inline std::vector<float> NetworkTableEntry::GetFloatArray(
    std::span<const float> defaultValue) const {
  return nt::GetFloatArray(m_handle, defaultValue);
}

inline std::vector<double> NetworkTableEntry::GetDoubleArray(
    std::span<const double> defaultValue) const {
  return nt::GetDoubleArray(m_handle, defaultValue);
}

inline std::vector<std::string> NetworkTableEntry::GetStringArray(
    std::span<const std::string> defaultValue) const {
  return nt::GetStringArray(m_handle, defaultValue);
}

inline std::vector<NetworkTableValue> NetworkTableEntry::ReadQueue() {
  return nt::ReadQueueValue(m_handle);
}

inline bool NetworkTableEntry::SetDefaultValue(const Value& defaultValue) {
  return SetDefaultEntryValue(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultBoolean(bool defaultValue) {
  return nt::SetDefaultBoolean(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultInteger(int64_t defaultValue) {
  return nt::SetDefaultInteger(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultFloat(float defaultValue) {
  return nt::SetDefaultFloat(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultDouble(double defaultValue) {
  return nt::SetDefaultDouble(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultString(std::string_view defaultValue) {
  return nt::SetDefaultString(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultRaw(
    std::span<const uint8_t> defaultValue) {
  return nt::SetDefaultRaw(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultBooleanArray(
    std::span<const int> defaultValue) {
  return nt::SetDefaultBooleanArray(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultIntegerArray(
    std::span<const int64_t> defaultValue) {
  return nt::SetDefaultIntegerArray(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultFloatArray(
    std::span<const float> defaultValue) {
  return nt::SetDefaultFloatArray(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultDoubleArray(
    std::span<const double> defaultValue) {
  return nt::SetDefaultDoubleArray(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetDefaultStringArray(
    std::span<const std::string> defaultValue) {
  return nt::SetDefaultStringArray(m_handle, defaultValue);
}

inline bool NetworkTableEntry::SetValue(const Value& value) {
  return SetEntryValue(m_handle, value);
}

inline bool NetworkTableEntry::SetBoolean(bool value, int64_t time) {
  return nt::SetBoolean(m_handle, value, time);
}

inline bool NetworkTableEntry::SetInteger(int64_t value, int64_t time) {
  return nt::SetInteger(m_handle, value, time);
}

inline bool NetworkTableEntry::SetFloat(float value, int64_t time) {
  return nt::SetFloat(m_handle, value, time);
}

inline bool NetworkTableEntry::SetDouble(double value, int64_t time) {
  return nt::SetDouble(m_handle, value, time);
}

inline bool NetworkTableEntry::SetString(std::string_view value, int64_t time) {
  return nt::SetString(m_handle, value, time);
}

inline bool NetworkTableEntry::SetRaw(std::span<const uint8_t> value,
                                      int64_t time) {
  return nt::SetRaw(m_handle, value, time);
}

inline bool NetworkTableEntry::SetBooleanArray(std::span<const bool> value,
                                               int64_t time) {
  return SetEntryValue(m_handle, Value::MakeBooleanArray(value, time));
}

inline bool NetworkTableEntry::SetBooleanArray(std::span<const int> value,
                                               int64_t time) {
  return nt::SetBooleanArray(m_handle, value, time);
}

inline bool NetworkTableEntry::SetIntegerArray(std::span<const int64_t> value,
                                               int64_t time) {
  return nt::SetIntegerArray(m_handle, value, time);
}

inline bool NetworkTableEntry::SetFloatArray(std::span<const float> value,
                                             int64_t time) {
  return nt::SetFloatArray(m_handle, value, time);
}

inline bool NetworkTableEntry::SetDoubleArray(std::span<const double> value,
                                              int64_t time) {
  return nt::SetDoubleArray(m_handle, value, time);
}

inline bool NetworkTableEntry::SetStringArray(
    std::span<const std::string> value, int64_t time) {
  return nt::SetStringArray(m_handle, value, time);
}

inline void NetworkTableEntry::SetFlags(unsigned int flags) {
  SetEntryFlags(m_handle, GetEntryFlags(m_handle) | flags);
}

inline void NetworkTableEntry::ClearFlags(unsigned int flags) {
  SetEntryFlags(m_handle, GetEntryFlags(m_handle) & ~flags);
}

inline void NetworkTableEntry::SetPersistent() {
  nt::SetTopicPersistent(nt::GetTopicFromHandle(m_handle), true);
}

inline void NetworkTableEntry::ClearPersistent() {
  nt::SetTopicPersistent(nt::GetTopicFromHandle(m_handle), false);
}

inline bool NetworkTableEntry::IsPersistent() const {
  return nt::GetTopicPersistent(nt::GetTopicFromHandle(m_handle));
}

inline void NetworkTableEntry::Unpublish() {
  return nt::Unpublish(m_handle);
}

inline void NetworkTableEntry::Delete() {
  Unpublish();
}

}  // namespace nt
