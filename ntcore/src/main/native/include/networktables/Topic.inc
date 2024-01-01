// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "networktables/NetworkTableType.h"
#include "networktables/Topic.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

inline std::string Topic::GetName() const {
  return ::nt::GetTopicName(m_handle);
}

inline NetworkTableType Topic::GetType() const {
  return static_cast<NetworkTableType>(::nt::GetTopicType(m_handle));
}

inline std::string Topic::GetTypeString() const {
  return ::nt::GetTopicTypeString(m_handle);
}

inline void Topic::SetPersistent(bool persistent) {
  ::nt::SetTopicPersistent(m_handle, persistent);
}

inline bool Topic::IsPersistent() const {
  return ::nt::GetTopicPersistent(m_handle);
}

inline void Topic::SetRetained(bool retained) {
  ::nt::SetTopicRetained(m_handle, retained);
}

inline bool Topic::IsRetained() const {
  return ::nt::GetTopicRetained(m_handle);
}

inline void Topic::SetCached(bool cached) {
  ::nt::SetTopicCached(m_handle, cached);
}

inline bool Topic::IsCached() const {
  return ::nt::GetTopicCached(m_handle);
}

inline bool Topic::Exists() const {
  return nt::GetTopicExists(m_handle);
}

inline void Topic::DeleteProperty(std::string_view name) {
  ::nt::DeleteTopicProperty(m_handle, name);
}

inline bool Topic::SetProperties(const wpi::json& properties) {
  return ::nt::SetTopicProperties(m_handle, properties);
}

inline TopicInfo Topic::GetInfo() const {
  return ::nt::GetTopicInfo(m_handle);
}

inline Subscriber::~Subscriber() {
  ::nt::Release(m_subHandle);
}

inline Subscriber::Subscriber(Subscriber&& rhs) : m_subHandle{rhs.m_subHandle} {
  rhs.m_subHandle = 0;
}

inline Subscriber& Subscriber::operator=(Subscriber&& rhs) {
  if (m_subHandle != 0) {
    ::nt::Release(m_subHandle);
  }
  m_subHandle = rhs.m_subHandle;
  rhs.m_subHandle = 0;
  return *this;
}

inline bool Subscriber::Exists() const {
  return nt::GetTopicExists(m_subHandle);
}

inline int64_t Subscriber::GetLastChange() const {
  return ::nt::GetEntryLastChange(m_subHandle);
}

inline Topic Subscriber::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_subHandle)};
}

inline Publisher::~Publisher() {
  ::nt::Release(m_pubHandle);
}

inline Publisher::Publisher(Publisher&& rhs) : m_pubHandle{rhs.m_pubHandle} {
  rhs.m_pubHandle = 0;
}

inline Publisher& Publisher::operator=(Publisher&& rhs) {
  if (m_pubHandle != 0) {
    ::nt::Release(m_pubHandle);
  }
  m_pubHandle = rhs.m_pubHandle;
  rhs.m_pubHandle = 0;
  return *this;
}

inline Topic Publisher::GetTopic() const {
  return Topic{::nt::GetTopicFromHandle(m_pubHandle)};
}

}  // namespace nt
