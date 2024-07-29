// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "networktables/GenericEntry.h"
#include "networktables/NetworkTableType.h"
#include "ntcore_cpp.h"

namespace nt {

inline GenericSubscriber::GenericSubscriber(NT_Subscriber handle)
    : Subscriber{handle} {}

inline Value GenericSubscriber::Get() const {
  return ::nt::GetEntryValue(m_subHandle);
}

inline bool GenericSubscriber::GetBoolean(bool defaultValue) const {
  return ::nt::GetBoolean(m_subHandle, defaultValue);
}

inline int64_t GenericSubscriber::GetInteger(int64_t defaultValue) const {
  return ::nt::GetInteger(m_subHandle, defaultValue);
}

inline float GenericSubscriber::GetFloat(float defaultValue) const {
  return ::nt::GetFloat(m_subHandle, defaultValue);
}

inline double GenericSubscriber::GetDouble(double defaultValue) const {
  return ::nt::GetDouble(m_subHandle, defaultValue);
}

inline std::string GenericSubscriber::GetString(
    std::string_view defaultValue) const {
  return ::nt::GetString(m_subHandle, defaultValue);
}

inline std::vector<uint8_t> GenericSubscriber::GetRaw(
    std::span<const uint8_t> defaultValue) const {
  return ::nt::GetRaw(m_subHandle, defaultValue);
}

inline std::vector<int> GenericSubscriber::GetBooleanArray(
    std::span<const int> defaultValue) const {
  return ::nt::GetBooleanArray(m_subHandle, defaultValue);
}

inline std::vector<int64_t> GenericSubscriber::GetIntegerArray(
    std::span<const int64_t> defaultValue) const {
  return ::nt::GetIntegerArray(m_subHandle, defaultValue);
}

inline std::vector<float> GenericSubscriber::GetFloatArray(
    std::span<const float> defaultValue) const {
  return ::nt::GetFloatArray(m_subHandle, defaultValue);
}

inline std::vector<double> GenericSubscriber::GetDoubleArray(
    std::span<const double> defaultValue) const {
  return ::nt::GetDoubleArray(m_subHandle, defaultValue);
}

inline std::vector<std::string> GenericSubscriber::GetStringArray(
    std::span<const std::string> defaultValue) const {
  return ::nt::GetStringArray(m_subHandle, defaultValue);
}

inline std::vector<Value> GenericSubscriber::ReadQueue() {
  return ::nt::ReadQueueValue(m_subHandle);
}

inline Topic GenericSubscriber::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_subHandle)};
}

inline GenericPublisher::GenericPublisher(NT_Publisher handle)
    : Publisher{handle} {}

inline void GenericPublisher::Set(const Value& value) {
  ::nt::SetEntryValue(m_pubHandle, value);
}

inline bool GenericPublisher::SetBoolean(bool value, int64_t time) {
  return nt::SetBoolean(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetInteger(int64_t value, int64_t time) {
  return nt::SetInteger(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetFloat(float value, int64_t time) {
  return nt::SetFloat(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetDouble(double value, int64_t time) {
  return nt::SetDouble(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetString(std::string_view value, int64_t time) {
  return nt::SetString(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetRaw(std::span<const uint8_t> value,
                                     int64_t time) {
  return nt::SetRaw(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetBooleanArray(std::span<const bool> value,
                                              int64_t time) {
  return SetEntryValue(m_pubHandle, Value::MakeBooleanArray(value, time));
}

inline bool GenericPublisher::SetBooleanArray(std::span<const int> value,
                                              int64_t time) {
  return nt::SetBooleanArray(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetIntegerArray(std::span<const int64_t> value,
                                              int64_t time) {
  return nt::SetIntegerArray(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetFloatArray(std::span<const float> value,
                                            int64_t time) {
  return nt::SetFloatArray(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetDoubleArray(std::span<const double> value,
                                             int64_t time) {
  return nt::SetDoubleArray(m_pubHandle, value, time);
}

inline bool GenericPublisher::SetStringArray(std::span<const std::string> value,
                                             int64_t time) {
  return nt::SetStringArray(m_pubHandle, value, time);
}

inline void GenericPublisher::SetDefault(const Value& value) {
  ::nt::SetDefaultEntryValue(m_pubHandle, value);
}

inline bool GenericPublisher::SetDefaultBoolean(bool defaultValue) {
  return nt::SetDefaultBoolean(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultInteger(int64_t defaultValue) {
  return nt::SetDefaultInteger(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultFloat(float defaultValue) {
  return nt::SetDefaultFloat(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultDouble(double defaultValue) {
  return nt::SetDefaultDouble(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultString(std::string_view defaultValue) {
  return nt::SetDefaultString(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultRaw(
    std::span<const uint8_t> defaultValue) {
  return nt::SetDefaultRaw(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultBooleanArray(
    std::span<const int> defaultValue) {
  return nt::SetDefaultBooleanArray(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultIntegerArray(
    std::span<const int64_t> defaultValue) {
  return nt::SetDefaultIntegerArray(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultFloatArray(
    std::span<const float> defaultValue) {
  return nt::SetDefaultFloatArray(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultDoubleArray(
    std::span<const double> defaultValue) {
  return nt::SetDefaultDoubleArray(m_pubHandle, defaultValue);
}

inline bool GenericPublisher::SetDefaultStringArray(
    std::span<const std::string> defaultValue) {
  return nt::SetDefaultStringArray(m_pubHandle, defaultValue);
}

inline Topic GenericPublisher::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_pubHandle)};
}

inline GenericEntry::GenericEntry(NT_Entry handle)
    : GenericSubscriber{handle}, GenericPublisher{handle} {}

inline Topic GenericEntry::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_subHandle)};
}

inline void GenericEntry::Unpublish() {
  ::nt::Unpublish(m_pubHandle);
}
}  // namespace nt
