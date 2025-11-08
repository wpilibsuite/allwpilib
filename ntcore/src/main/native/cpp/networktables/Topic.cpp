// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/Topic.hpp"

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::nt;

NetworkTableInstance Topic::GetInstance() const {
  return NetworkTableInstance{GetInstanceFromHandle(m_handle)};
}

wpi::util::json Topic::GetProperty(std::string_view name) const {
  return ::wpi::nt::GetTopicProperty(m_handle, name);
}

void Topic::SetProperty(std::string_view name, const wpi::util::json& value) {
  ::wpi::nt::SetTopicProperty(m_handle, name, value);
}

wpi::util::json Topic::GetProperties() const {
  return ::wpi::nt::GetTopicProperties(m_handle);
}

GenericSubscriber Topic::GenericSubscribe(const PubSubOptions& options) {
  return GenericSubscribe("", options);
}

GenericSubscriber Topic::GenericSubscribe(std::string_view typeString,
                                          const PubSubOptions& options) {
  return GenericSubscriber{::wpi::nt::Subscribe(
      m_handle, ::wpi::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublish(std::string_view typeString,
                                       const PubSubOptions& options) {
  return GenericPublisher{::wpi::nt::Publish(
      m_handle, ::wpi::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublishEx(std::string_view typeString,
                                         const wpi::util::json& properties,
                                         const PubSubOptions& options) {
  return GenericPublisher{::wpi::nt::PublishEx(m_handle,
                                          ::wpi::nt::GetTypeFromString(typeString),
                                          typeString, properties, options)};
}

GenericEntry Topic::GetGenericEntry(const PubSubOptions& options) {
  return GetGenericEntry("", options);
}

GenericEntry Topic::GetGenericEntry(std::string_view typeString,
                                    const PubSubOptions& options) {
  return GenericEntry{::wpi::nt::GetEntry(
      m_handle, ::wpi::nt::GetTypeFromString(typeString), typeString, options)};
}

void Publisher::anchor() {}

void Subscriber::anchor() {}
