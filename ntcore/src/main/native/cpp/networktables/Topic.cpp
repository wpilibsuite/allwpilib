// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/Topic.h"

#include <wpi/json.h>

#include "networktables/GenericEntry.h"
#include "networktables/NetworkTableInstance.h"

using namespace nt;

NetworkTableInstance Topic::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}

wpi::json Topic::GetProperty(std::string_view name) const {
  return ::nt::GetTopicProperty(m_handle, name);
}

void Topic::SetProperty(std::string_view name, const wpi::json& value) {
  ::nt::SetTopicProperty(m_handle, name, value);
}

wpi::json Topic::GetProperties() const {
  return ::nt::GetTopicProperties(m_handle);
}

GenericSubscriber Topic::GenericSubscribe(const PubSubOptions& options) {
  return GenericSubscribe("", options);
}

GenericSubscriber Topic::GenericSubscribe(std::string_view typeString,
                                          const PubSubOptions& options) {
  return GenericSubscriber{::nt::Subscribe(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublish(std::string_view typeString,
                                       const PubSubOptions& options) {
  return GenericPublisher{::nt::Publish(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublishEx(std::string_view typeString,
                                         const wpi::json& properties,
                                         const PubSubOptions& options) {
  return GenericPublisher{::nt::PublishEx(m_handle,
                                          ::nt::GetTypeFromString(typeString),
                                          typeString, properties, options)};
}

GenericEntry Topic::GetGenericEntry(const PubSubOptions& options) {
  return GetGenericEntry("", options);
}

GenericEntry Topic::GetGenericEntry(std::string_view typeString,
                                    const PubSubOptions& options) {
  return GenericEntry{::nt::GetEntry(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}

void Publisher::anchor() {}

void Subscriber::anchor() {}
