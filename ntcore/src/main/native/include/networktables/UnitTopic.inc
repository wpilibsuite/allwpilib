// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <wpi/json.h>

#include "networktables/NetworkTableType.h"
#include "networktables/UnitTopic.h"
#include "ntcore_cpp.h"

namespace nt {

template <typename T>
inline UnitSubscriber<T>::UnitSubscriber(NT_Subscriber handle, T defaultValue)
    : Subscriber{handle}, m_defaultValue{defaultValue} {}

template <typename T>
inline T UnitSubscriber<T>::Get() const {
  return Get(m_defaultValue);
}

template <typename T>
inline T UnitSubscriber<T>::Get(T defaultValue) const {
  return T{::nt::GetDouble(m_subHandle, defaultValue.value())};
}

template <typename T>
inline TimestampedUnit<T> UnitSubscriber<T>::GetAtomic() const {
  return GetAtomic(m_defaultValue);
}

template <typename T>
inline TimestampedUnit<T> UnitSubscriber<T>::GetAtomic(T defaultValue) const {
  auto doubleVal = ::nt::GetAtomicDouble(m_subHandle, defaultValue.value());
  return {doubleVal.time, doubleVal.serverTime, doubleVal.value};
}

template <typename T>
inline std::vector<TimestampedUnit<T>> UnitSubscriber<T>::ReadQueue() {
  std::vector<TimestampedUnit<T>> vals;
  auto doubleVals = ::nt::ReadQueueDouble(m_subHandle);
  vals.reserve(doubleVals.size());
  for (auto&& val : doubleVals) {
    vals.emplace_back(val.time, val.serverTime, val.value);
  }
}

template <typename T>
inline UnitTopic<T> UnitSubscriber<T>::GetTopic() const {
  return UnitTopic<T>{::nt::GetTopicFromHandle(m_subHandle)};
}

template <typename T>
inline UnitPublisher<T>::UnitPublisher(NT_Publisher handle)
    : Publisher{handle} {}

template <typename T>
inline void UnitPublisher<T>::Set(T value, int64_t time) {
  ::nt::SetDouble(m_pubHandle, value.value(), time);
}

template <typename T>
inline void UnitPublisher<T>::SetDefault(T value) {
  ::nt::SetDefaultDouble(m_pubHandle, value.value());
}

template <typename T>
inline UnitTopic<T> UnitPublisher<T>::GetTopic() const {
  return UnitTopic<T>{::nt::GetTopicFromHandle(m_pubHandle)};
}

template <typename T>
inline UnitEntry<T>::UnitEntry(NT_Entry handle, T defaultValue)
    : UnitSubscriber<T>{handle, defaultValue}, UnitPublisher<T>{handle} {}

template <typename T>
inline UnitTopic<T> UnitEntry<T>::GetTopic() const {
  return UnitTopic<T>{::nt::GetTopicFromHandle(this->m_subHandle)};
}

template <typename T>
inline void UnitEntry<T>::Unpublish() {
  ::nt::Unpublish(this->m_pubHandle);
}

template <typename T>
inline bool UnitTopic<T>::IsMatchingUnit() const {
  return GetProperty("unit") == T{}.name();
}

template <typename T>
inline UnitSubscriber<T> UnitTopic<T>::Subscribe(T defaultValue,
                                                 const PubSubOptions& options) {
  return UnitSubscriber<T>{
      ::nt::Subscribe(m_handle, NT_DOUBLE, "double", options), defaultValue};
}

template <typename T>
inline UnitSubscriber<T> UnitTopic<T>::SubscribeEx(
    std::string_view typeString, T defaultValue, const PubSubOptions& options) {
  return UnitSubscriber<T>{
      ::nt::Subscribe(m_handle, NT_DOUBLE, typeString, options), defaultValue};
}

template <typename T>
inline UnitPublisher<T> UnitTopic<T>::Publish(const PubSubOptions& options) {
  return UnitPublisher<T>{::nt::PublishEx(m_handle, NT_DOUBLE, "double",
                                          {{"unit", T{}.name()}}, options)};
}

template <typename T>
inline UnitPublisher<T> UnitTopic<T>::PublishEx(std::string_view typeString,
                                                const wpi::json& properties,
                                                const PubSubOptions& options) {
  wpi::json props = properties;
  props["unit"] = T{}.name();
  return UnitPublisher<T>{
      ::nt::PublishEx(m_handle, NT_DOUBLE, typeString, props, options)};
}

template <typename T>
inline UnitEntry<T> UnitTopic<T>::GetEntry(T defaultValue,
                                           const PubSubOptions& options) {
  return UnitEntry<T>{::nt::GetEntry(m_handle, NT_DOUBLE, "double", options),
                      defaultValue};
}

template <typename T>
inline UnitEntry<T> UnitTopic<T>::GetEntryEx(std::string_view typeString,
                                             T defaultValue,
                                             const PubSubOptions& options) {
  return UnitEntry<T>{::nt::GetEntry(m_handle, NT_DOUBLE, typeString, options),
                      defaultValue};
}

}  // namespace nt
