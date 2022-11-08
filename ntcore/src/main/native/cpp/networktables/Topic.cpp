// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/Topic.h"

#include <wpi/json.h>

#include "networktables/GenericEntry.h"

using namespace nt;

wpi::json Topic::GetProperty(std::string_view name) const {
  return ::nt::GetTopicProperty(m_handle, name);
}

void Topic::SetProperty(std::string_view name, const wpi::json& value) {
  ::nt::SetTopicProperty(m_handle, name, value);
}

wpi::json Topic::GetProperties() const {
  return ::nt::GetTopicProperties(m_handle);
}

GenericSubscriber Topic::GenericSubscribe(
    std::span<const PubSubOption> options) {
  return GenericSubscribe("", options);
}

GenericSubscriber Topic::GenericSubscribe(
    std::string_view typeString, std::span<const PubSubOption> options) {
  return GenericSubscriber{::nt::Subscribe(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublish(std::string_view typeString,
                                       std::span<const PubSubOption> options) {
  return GenericPublisher{::nt::Publish(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}

GenericPublisher Topic::GenericPublishEx(
    std::string_view typeString, const wpi::json& properties,
    std::span<const PubSubOption> options) {
  return GenericPublisher{::nt::PublishEx(m_handle,
                                          ::nt::GetTypeFromString(typeString),
                                          typeString, properties, options)};
}

GenericEntry Topic::GetGenericEntry(std::span<const PubSubOption> options) {
  return GetGenericEntry("", options);
}

GenericEntry Topic::GetGenericEntry(std::string_view typeString,
                                    std::span<const PubSubOption> options) {
  return GenericEntry{::nt::GetEntry(
      m_handle, ::nt::GetTypeFromString(typeString), typeString, options)};
}
